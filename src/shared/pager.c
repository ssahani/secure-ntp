/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "sd-login.h"

#include "copy.h"
#include "env-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "io-util.h"
#include "locale-util.h"
#include "log.h"
#include "macro.h"
#include "pager.h"
#include "process-util.h"
#include "rlimit-util.h"
#include "signal-util.h"
#include "string-util.h"
#include "strv.h"
#include "terminal-util.h"
#include "util.h"

static pid_t pager_pid = 0;

static int stored_stdout = -1;
static int stored_stderr = -1;
static bool stdout_redirected = false;
static bool stderr_redirected = false;

static int no_quit_on_interrupt(int exe_name_fd, const char *less_opts) {
        _cleanup_fclose_ FILE *file = NULL;
        _cleanup_free_ char *line = NULL;
        int r;

        assert(exe_name_fd >= 0);
        assert(less_opts);

        /* This takes ownership of exe_name_fd */
        file = fdopen(exe_name_fd, "r");
        if (!file) {
                safe_close(exe_name_fd);
                return log_error_errno(errno, "Failed to create FILE object: %m");
        }

        /* Find the last line */
        for (;;) {
                _cleanup_free_ char *t = NULL;

                r = read_line(file, LONG_LINE_MAX, &t);
                if (r < 0)
                        return log_error_errno(r, "Failed to read from socket: %m");
                if (r == 0)
                        break;

                free_and_replace(line, t);
        }

        /* We only treat "less" specially.
         * Return true whenever option K is *not* set. */
        r = streq_ptr(line, "less") && !strchr(less_opts, 'K');

        log_debug("Pager executable is \"%s\", options \"%s\", quit_on_interrupt: %s",
                  strnull(line), less_opts, yes_no(!r));
        return r;
}

void pager_close(void) {

        if (pager_pid <= 0)
                return;

        /* Inform pager that we are done */
        (void) fflush(stdout);
        if (stdout_redirected)
                if (stored_stdout < 0 || dup2(stored_stdout, STDOUT_FILENO) < 0)
                        (void) close(STDOUT_FILENO);
        stored_stdout = safe_close(stored_stdout);
        (void) fflush(stderr);
        if (stderr_redirected)
                if (stored_stderr < 0 || dup2(stored_stderr, STDERR_FILENO) < 0)
                        (void) close(STDERR_FILENO);
        stored_stderr = safe_close(stored_stderr);
        stdout_redirected = stderr_redirected = false;

        (void) kill(pager_pid, SIGCONT);
        (void) wait_for_terminate(pager_pid, NULL);
        pager_pid = 0;
}

bool pager_have(void) {
        return pager_pid > 0;
}

int show_man_page(const char *desc, bool null_stdio) {
        const char *args[4] = { "man", NULL, NULL, NULL };
        char *e = NULL;
        pid_t pid;
        size_t k;
        int r;

        k = strlen(desc);

        if (desc[k-1] == ')')
                e = strrchr(desc, '(');

        if (e) {
                char *page = NULL, *section = NULL;

                page = strndupa(desc, e - desc);
                section = strndupa(e + 1, desc + k - e - 2);

                args[1] = section;
                args[2] = page;
        } else
                args[1] = desc;

        r = safe_fork("(man)", FORK_RESET_SIGNALS|FORK_DEATHSIG|(null_stdio ? FORK_NULL_STDIO : 0)|FORK_RLIMIT_NOFILE_SAFE|FORK_LOG, &pid);
        if (r < 0)
                return r;
        if (r == 0) {
                /* Child */
                execvp(args[0], (char**) args);
                log_error_errno(errno, "Failed to execute man: %m");
                _exit(EXIT_FAILURE);
        }

        return wait_for_terminate_and_check(NULL, pid, 0);
}
