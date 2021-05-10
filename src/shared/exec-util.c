/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <dirent.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "alloc-util.h"
#include "conf-files.h"
#include "env-file.h"
#include "env-util.h"
#include "errno-util.h"
#include "exec-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "hashmap.h"
#include "macro.h"
#include "missing_syscall.h"
#include "process-util.h"
#include "rlimit-util.h"
#include "serialize.h"
#include "set.h"
#include "signal-util.h"
#include "stat-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "terminal-util.h"
#include "tmpfile-util.h"
#include "util.h"

/* Put this test here for a lack of better place */
assert_cc(EAGAIN == EWOULDBLOCK);

static int do_spawn(const char *path, char *argv[], int stdout_fd, pid_t *pid, bool set_systemd_exec_pid) {
        pid_t _pid;
        int r;

        if (null_or_empty_path(path)) {
                log_debug("%s is empty (a mask).", path);
                return 0;
        }

        r = safe_fork("(direxec)", FORK_DEATHSIG|FORK_LOG, &_pid);
        if (r < 0)
                return r;
        if (r == 0) {
                char *_argv[2];

                if (stdout_fd >= 0) {
                        r = rearrange_stdio(STDIN_FILENO, stdout_fd, STDERR_FILENO);
                        if (r < 0)
                                _exit(EXIT_FAILURE);
                }

                (void) rlimit_nofile_safe();

                if (set_systemd_exec_pid) {
                        r = setenv_systemd_exec_pid(false);
                        if (r < 0)
                                log_warning_errno(r, "Failed to set $SYSTEMD_EXEC_PID, ignoring: %m");
                }

                if (!argv) {
                        _argv[0] = (char*) path;
                        _argv[1] = NULL;
                        argv = _argv;
                } else
                        argv[0] = (char*) path;

                execv(path, argv);
                log_error_errno(errno, "Failed to execute %s: %m", path);
                _exit(EXIT_FAILURE);
        }

        *pid = _pid;
        return 1;
}

static int do_execute(
                char **directories,
                usec_t timeout,
                gather_stdout_callback_t const callbacks[_STDOUT_CONSUME_MAX],
                void* const callback_args[_STDOUT_CONSUME_MAX],
                int output_fd,
                char *argv[],
                char *envp[],
                ExecDirFlags flags) {

        _cleanup_hashmap_free_free_ Hashmap *pids = NULL;
        _cleanup_strv_free_ char **paths = NULL;
        char **path, **e;
        int r;
        bool parallel_execution;

        /* We fork this all off from a child process so that we can somewhat cleanly make
         * use of SIGALRM to set a time limit.
         *
         * We attempt to perform parallel execution if configured by the user, however
         * if `callbacks` is nonnull, execution must be serial.
         */
        parallel_execution = FLAGS_SET(flags, EXEC_DIR_PARALLEL) && !callbacks;

        r = conf_files_list_strv(&paths, NULL, NULL, CONF_FILES_EXECUTABLE|CONF_FILES_REGULAR|CONF_FILES_FILTER_MASKED, (const char* const*) directories);
        if (r < 0)
                return log_error_errno(r, "Failed to enumerate executables: %m");

        if (parallel_execution) {
                pids = hashmap_new(NULL);
                if (!pids)
                        return log_oom();
        }

        /* Abort execution of this process after the timeout. We simply rely on SIGALRM as
         * default action terminating the process, and turn on alarm(). */

        if (timeout != USEC_INFINITY)
                alarm(DIV_ROUND_UP(timeout, USEC_PER_SEC));

        STRV_FOREACH(e, envp)
                if (putenv(*e) != 0)
                        return log_error_errno(errno, "Failed to set environment variable: %m");

        STRV_FOREACH(path, paths) {
                _cleanup_free_ char *t = NULL;
                _cleanup_close_ int fd = -1;
                pid_t pid;

                t = strdup(*path);
                if (!t)
                        return log_oom();

                if (callbacks) {
                        fd = open_serialization_fd(basename(*path));
                        if (fd < 0)
                                return log_error_errno(fd, "Failed to open serialization file: %m");
                }

                r = do_spawn(t, argv, fd, &pid, FLAGS_SET(flags, EXEC_DIR_SET_SYSTEMD_EXEC_PID));
                if (r <= 0)
                        continue;

                if (parallel_execution) {
                        r = hashmap_put(pids, PID_TO_PTR(pid), t);
                        if (r < 0)
                                return log_oom();
                        t = NULL;
                } else {
                        r = wait_for_terminate_and_check(t, pid, WAIT_LOG);
                        if (FLAGS_SET(flags, EXEC_DIR_IGNORE_ERRORS)) {
                                if (r < 0)
                                        continue;
                        } else if (r > 0)
                                return r;

                        if (callbacks) {
                                if (lseek(fd, 0, SEEK_SET) < 0)
                                        return log_error_errno(errno, "Failed to seek on serialization fd: %m");

                                r = callbacks[STDOUT_GENERATE](fd, callback_args[STDOUT_GENERATE]);
                                fd = -1;
                                if (r < 0)
                                        return log_error_errno(r, "Failed to process output from %s: %m", *path);
                        }
                }
        }

        if (callbacks) {
                r = callbacks[STDOUT_COLLECT](output_fd, callback_args[STDOUT_COLLECT]);
                if (r < 0)
                        return log_error_errno(r, "Callback two failed: %m");
        }

        while (!hashmap_isempty(pids)) {
                _cleanup_free_ char *t = NULL;
                pid_t pid;

                pid = PTR_TO_PID(hashmap_first_key(pids));
                assert(pid > 0);

                t = hashmap_remove(pids, PID_TO_PTR(pid));
                assert(t);

                r = wait_for_terminate_and_check(t, pid, WAIT_LOG);
                if (!FLAGS_SET(flags, EXEC_DIR_IGNORE_ERRORS) && r > 0)
                        return r;
        }

        return 0;
}

int execute_directories(
                const char* const* directories,
                usec_t timeout,
                gather_stdout_callback_t const callbacks[_STDOUT_CONSUME_MAX],
                void* const callback_args[_STDOUT_CONSUME_MAX],
                char *argv[],
                char *envp[],
                ExecDirFlags flags) {

        char **dirs = (char**) directories;
        _cleanup_close_ int fd = -1;
        char *name;
        int r;
        pid_t executor_pid;

        assert(!strv_isempty(dirs));

        name = basename(dirs[0]);
        assert(!isempty(name));

        if (callbacks) {
                assert(callback_args);
                assert(callbacks[STDOUT_GENERATE]);
                assert(callbacks[STDOUT_COLLECT]);
                assert(callbacks[STDOUT_CONSUME]);

                fd = open_serialization_fd(name);
                if (fd < 0)
                        return log_error_errno(fd, "Failed to open serialization file: %m");
        }

        /* Executes all binaries in the directories serially or in parallel and waits for
         * them to finish. Optionally a timeout is applied. If a file with the same name
         * exists in more than one directory, the earliest one wins. */

        r = safe_fork("(sd-executor)", FORK_RESET_SIGNALS|FORK_DEATHSIG|FORK_LOG, &executor_pid);
        if (r < 0)
                return r;
        if (r == 0) {
                r = do_execute(dirs, timeout, callbacks, callback_args, fd, argv, envp, flags);
                _exit(r < 0 ? EXIT_FAILURE : r);
        }

        r = wait_for_terminate_and_check("(sd-executor)", executor_pid, 0);
        if (r < 0)
                return r;
        if (!FLAGS_SET(flags, EXEC_DIR_IGNORE_ERRORS) && r > 0)
                return r;

        if (!callbacks)
                return 0;

        if (lseek(fd, 0, SEEK_SET) < 0)
                return log_error_errno(errno, "Failed to rewind serialization fd: %m");

        r = callbacks[STDOUT_CONSUME](fd, callback_args[STDOUT_CONSUME]);
        fd = -1;
        if (r < 0)
                return log_error_errno(r, "Failed to parse returned data: %m");
        return 0;
}

