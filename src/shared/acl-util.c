/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "acl-util.h"
#include "alloc-util.h"
#include "string-util.h"
#include "strv.h"
#include "user-util.h"
#include "util.h"

int acl_find_uid(acl_t acl, uid_t uid, acl_entry_t *ret_entry) {
        acl_entry_t i;
        int r;

        assert(acl);
        assert(uid_is_valid(uid));
        assert(ret_entry);

        for (r = acl_get_entry(acl, ACL_FIRST_ENTRY, &i);
             r > 0;
             r = acl_get_entry(acl, ACL_NEXT_ENTRY, &i)) {

                acl_tag_t tag;
                uid_t *u;
                bool b;

                if (acl_get_tag_type(i, &tag) < 0)
                        return -errno;

                if (tag != ACL_USER)
                        continue;

                u = acl_get_qualifier(i);
                if (!u)
                        return -errno;

                b = *u == uid;
                acl_free(u);

                if (b) {
                        *ret_entry = i;
                        return 1;
                }
        }
        if (r < 0)
                return -errno;

        return 0;
}

