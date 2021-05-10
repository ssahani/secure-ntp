/* SPDX-License-Identifier: LGPL-2.1+ */

#include <fcntl.h>

//#include "ask-password-api.h"
#include "escape.h"
#include "fd-util.h"
#include "io-util.h"
#include "memory-util.h"
#include "pkcs11-util.h"
#include "random-util.h"
#include "string-util.h"
#include "strv.h"

bool pkcs11_uri_valid(const char *uri) {
        const char *p;

        /* A very superficial checker for RFC7512 PKCS#11 URI syntax */

        if (isempty(uri))
                return false;

        p = startswith(uri, "pkcs11:");
        if (!p)
                return false;

        if (isempty(p))
                return false;

        if (!in_charset(p, ALPHANUMERICAL "-_?;&%="))
                return false;

        return true;
}
