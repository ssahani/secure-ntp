/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/xattr.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chattr-util.h"
#include "copy.h"
#include "dirent-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "io-util.h"
#include "macro.h"
#include "missing_syscall.h"
#include "mountpoint-util.h"
#include "nulstr-util.h"
#include "selinux-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "tmpfile-util.h"
#include "umask-util.h"
#include "user-util.h"
#include "xattr-util.h"

#define COPY_BUFFER_SIZE (16U*1024U)

/* A safety net for descending recursively into file system trees to copy. On Linux PATH_MAX is 4096, which means the
 * deepest valid path one can build is around 2048, which we hence use as a safety net here, to not spin endlessly in
 * case of bind mount cycles and suchlike. */
#define COPY_DEPTH_MAX 2048U

static ssize_t try_copy_file_range(
                int fd_in, loff_t *off_in,
                int fd_out, loff_t *off_out,
                size_t len,
                unsigned flags) {

        static int have = -1;
        ssize_t r;

        if (have == 0)
                return -ENOSYS;

        r = copy_file_range(fd_in, off_in, fd_out, off_out, len, flags);
        if (have < 0)
                have = r >= 0 || errno != ENOSYS;
        if (r < 0)
                return -errno;

        return r;
}

enum {
        FD_IS_NO_PIPE,
        FD_IS_BLOCKING_PIPE,
        FD_IS_NONBLOCKING_PIPE,
};

static int fd_is_nonblock_pipe(int fd) {
        struct stat st;
        int flags;

        /* Checks whether the specified file descriptor refers to a pipe, and if so if O_NONBLOCK is set. */

        if (fstat(fd, &st) < 0)
                return -errno;

        if (!S_ISFIFO(st.st_mode))
                return FD_IS_NO_PIPE;

        flags = fcntl(fd, F_GETFL);
        if (flags < 0)
                return -errno;

        return FLAGS_SET(flags, O_NONBLOCK) ? FD_IS_NONBLOCKING_PIPE : FD_IS_BLOCKING_PIPE;
}

static int sigint_pending(void) {
        sigset_t ss;

        assert_se(sigemptyset(&ss) >= 0);
        assert_se(sigaddset(&ss, SIGINT) >= 0);

        if (sigtimedwait(&ss, NULL, &(struct timespec) { 0, 0 }) < 0) {
                if (errno == EAGAIN)
                        return false;

                return -errno;
        }

        return true;
}

static int fd_copy_symlink(
                int df,
                const char *from,
                const struct stat *st,
                int dt,
                const char *to,
                uid_t override_uid,
                gid_t override_gid,
                CopyFlags copy_flags) {

        _cleanup_free_ char *target = NULL;
        int r;

        assert(from);
        assert(st);
        assert(to);

        r = readlinkat_malloc(df, from, &target);
        if (r < 0)
                return r;

        if (copy_flags & COPY_MAC_CREATE) {
                r = mac_selinux_create_file_prepare_at(dt, to, S_IFLNK);
                if (r < 0)
                        return r;
        }
        r = symlinkat(target, dt, to);
        if (copy_flags & COPY_MAC_CREATE)
                mac_selinux_create_file_clear();
        if (r < 0)
                return -errno;

        if (fchownat(dt, to,
                     uid_is_valid(override_uid) ? override_uid : st->st_uid,
                     gid_is_valid(override_gid) ? override_gid : st->st_gid,
                     AT_SYMLINK_NOFOLLOW) < 0)
                r = -errno;

        (void) utimensat(dt, to, (struct timespec[]) { st->st_atim, st->st_mtim }, AT_SYMLINK_NOFOLLOW);
        return r;
}

/* Encapsulates the database we store potential hardlink targets in */
typedef struct HardlinkContext {
        int dir_fd;    /* An fd to the directory we use as lookup table. Never AT_FDCWD. Lazily created, when
                        * we add the first entry. */

        /* These two fields are used to create the hardlink repository directory above — via
         * mkdirat(parent_fd, subdir) — and are kept so that we can automatically remove the directory again
         * when we are done. */
        int parent_fd; /* Possibly AT_FDCWD */
        char *subdir;
} HardlinkContext;

static int hardlink_context_setup(
                HardlinkContext *c,
                int dt,
                const char *to,
                CopyFlags copy_flags) {

        _cleanup_close_ int dt_copy = -1;
        int r;

        assert(c);
        assert(c->dir_fd < 0 && c->dir_fd != AT_FDCWD);
        assert(c->parent_fd < 0);
        assert(!c->subdir);

        /* If hardlink recreation is requested we have to maintain a database of inodes that are potential
         * hardlink sources. Given that generally disk sizes have to be assumed to be larger than what fits
         * into physical RAM we cannot maintain that database in dynamic memory alone. Here we opt to
         * maintain it on disk, to simplify things: inside the destination directory we'll maintain a
         * temporary directory consisting of hardlinks of every inode we copied that might be subject of
         * hardlinks. We can then use that as hardlink source later on. Yes, this means additional disk IO
         * but thankfully Linux is optimized for this kind of thing. If this ever becomes a performance
         * bottleneck we can certainly place an in-memory hash table in front of this, but for the beginning,
         * let's keep things simple, and just use the disk as lookup table for inodes.
         *
         * Note that this should have zero performance impact as long as .n_link of all files copied remains
         * <= 0, because in that case we will not actually allocate the hardlink inode lookup table directory
         * on disk (we do so lazily, when the first candidate with .n_link > 1 is seen). This means, in the
         * common case where hardlinks are not used at all or only for few files the fact that we store the
         * table on disk shouldn't matter perfomance-wise. */

        if (!FLAGS_SET(copy_flags, COPY_HARDLINKS))
                return 0;

        if (dt == AT_FDCWD)
                dt_copy = AT_FDCWD;
        else if (dt < 0)
                return -EBADF;
        else {
                dt_copy = fcntl(dt, F_DUPFD_CLOEXEC, 3);
                if (dt_copy < 0)
                        return -errno;
        }

        r = tempfn_random_child(to, "hardlink", &c->subdir);
        if (r < 0)
                return r;

        c->parent_fd = TAKE_FD(dt_copy);

        /* We don't actually create the directory we keep the table in here, that's done on-demand when the
         * first entry is added, using hardlink_context_realize() below. */
        return 1;
}

static int hardlink_context_realize(HardlinkContext *c) {
        int r;

        if (!c)
                return 0;

        if (c->dir_fd >= 0) /* Already realized */
                return 1;

        if (c->parent_fd < 0 && c->parent_fd != AT_FDCWD) /* Not configured */
                return 0;

        assert(c->subdir);

        if (mkdirat(c->parent_fd, c->subdir, 0700) < 0)
                return -errno;

        c->dir_fd = openat(c->parent_fd, c->subdir, O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        if (c->dir_fd < 0) {
                r = -errno;
                (void) unlinkat(c->parent_fd, c->subdir, AT_REMOVEDIR);
                return r;
        }

        return 1;
}
