# SPDX-License-Identifier: LGPL-2.1-or-later

shared_sources = files('''
        acl-util.h
        acpi-fpdt.c
        acpi-fpdt.h
        base-filesystem.c
        base-filesystem.h
        binfmt-util.c
        binfmt-util.h
        bitmap.c
        bitmap.h
        calendarspec.c
        calendarspec.h
        chown-recursive.c
        chown-recursive.h
        clock-util.c
        clock-util.h
        conf-parser.c
        conf-parser.h
        cpu-set-util.c
        cpu-set-util.h
        daemon-util.h
        dev-setup.c
        dev-setup.h
        dm-util.c
        dm-util.h
        dns-domain.c
        dns-domain.h
        enable-mempool.c
        ethtool-util.c
        ethtool-util.h
        exec-util.c
        exec-util.h
        exit-status.c
        exit-status.h
        extension-release.c
        extension-release.h
        fdset.c
        fdset.h
        fileio-label.c
        fileio-label.h
        fsck-util.h
        id128-print.c
        id128-print.h
        ima-util.c
        ima-util.h
        initreq.h
        linux/auto_dev-ioctl.h
        linux/bpf.h
        linux/bpf_common.h
        linux/bpf_insn.h
        linux/dm-ioctl.h
        linux/ethtool.h
        lockfile-util.c
        lockfile-util.h
        log-link.h
        main-func.h
        module-util.h
        nscd-flush.h
        nsflags.c
        nsflags.h
        numa-util.c
        numa-util.h
        pager.c
        pager.h
        pe-header.h
        pkcs11-util.c
        pkcs11-util.h
        pretty-print.c
        pretty-print.h
        psi-util.c
        psi-util.h
        resolve-util.c
        resolve-util.h
        securebits-util.c
        securebits-util.h
        serialize.c
        serialize.h
        tmpfile-util-label.c
        tmpfile-util-label.h
        uid-range.c
        verbs.c
        verbs.h
        volatile-util.c
        volatile-util.h
        xml.c
        xml.h
'''.split())

libshared_name = 'systemd-shared-@0@'.format(meson.project_version())

libshared_deps = [threads,
                  libcap,
                  librt,
                  libselinux]

libshared_sym_path = '@0@/libshared.sym'.format(meson.current_source_dir())

libshared_static = static_library(
        libshared_name,
        shared_sources,
        include_directories : includes,
        dependencies : libshared_deps,
        c_args : ['-fvisibility=default'])
