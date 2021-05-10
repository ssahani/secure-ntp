/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <stdbool.h>
#include <sys/types.h>

#include "time-util.h"
#include "util.h"

#if ENABLE_UTMP
#include <utmpx.h>

int utmp_put_shutdown(void);
int utmp_put_reboot(usec_t timestamp);
int utmp_wall(
        const char *message,
        const char *username,
        const char *origin_tty,
        bool (*match_tty)(const char *tty, void *userdata),
        void *userdata);

static inline bool utxent_start(void) {
        setutxent();
        return true;
}
static inline void utxent_cleanup(bool *initialized) {
        if (initialized)
                endutxent();
}

static inline int utmp_put_shutdown(void) {
        return 0;
}
static inline int utmp_put_reboot(usec_t timestamp) {
        return 0;
}

static inline int utmp_wall(
                const char *message,
                const char *username,
                const char *origin_tty,
                bool (*match_tty)(const char *tty, void *userdata),
                void *userdata) {
        return 0;
}

