/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "build.h"

const char* const systemd_features =

#if HAVE_GNUTLS
        " +GNUTLS"
#else
        " -GNUTLS"
#endif
        ;
