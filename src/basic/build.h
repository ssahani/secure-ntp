/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

#include "version.h"

#if HAVE_GNUTLS
#define _GNUTLS_FEATURE_ "+GNUTLS"
#else
#define _GNUTLS_FEATURE_ "-GNUTLS"
#endif

#define SYSTEMD_FEATURES                                                \
        _GNUTLS_FEATURE_
