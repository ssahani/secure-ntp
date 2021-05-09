/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "build.h"

const char* const systemd_features =

#if HAVE_PAM
        "+PAM"
#else
        "-PAM"
#endif

#if HAVE_GNUTLS
        " +GNUTLS"
#else
        " -GNUTLS"
#endif

#if HAVE_OPENSSL
        " +OPENSSL"
#else
        " -OPENSSL"
#endif

        " default-hierarchy=" DEFAULT_HIERARCHY_NAME
        ;
