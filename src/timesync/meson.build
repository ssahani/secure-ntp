# SPDX-License-Identifier: LGPL-2.1-or-later

sources = files(
        'timesyncd-conf.c',
        'timesyncd-manager.c',
        'timesyncd-manager.h',
        'timesyncd-ntp-message.h',
        'timesyncd-nts-client.c',
        'timesyncd-nts-client.h',
        'timesyncd-ntp-extension.c',
        'timesyncd-ntp-extension.h',
        'timesyncd-ntske-client.c',
        'timesyncd-ntske-client.h',
        'timesyncd-ntske-protocol.c',
        'timesyncd-ntske-protocol.h',
        'timesyncd-server.c',
)

nts_timesyncd_sources = files(
        'timesyncd.c',
        'timesyncd-bus.c',
)

sources += custom_target(
        'nts-timesyncd-gperf.c',
        input : 'nts-timesyncd-gperf.gperf',
        output : 'nts-timesyncd-gperf.c',
        command : [gperf, '@INPUT@', '--output-file', '@OUTPUT@'])

nts_timesyncd_link_with = [libsystemd_static,
                           libshared_static,
                           libbasic_gcrypt]

nts_libtimesyncd_core = static_library(
        'timesyncd-core',
        sources,
        include_directories : includes,
        dependencies : userspace,
        link_with : nts_timesyncd_link_with,
        build_by_default : false)

custom_target(
        'nts-timesyncd.conf',
        input : 'nts-timesyncd.conf.in',
        output : 'nts-timesyncd.conf',
        command : [jinja2_cmdline, '@INPUT@', '@OUTPUT@'],
        install : true,
        install_dir : pkgsysconfdir)

        install_data('org.freedesktop.ntstimesync1.conf',
                     install_dir : dbuspolicydir)
        install_data('org.freedesktop.ntstimesync1.service',
                     install_dir : dbussystemservicedir)
        install_data('80-systemd-ntstimesync.list',
                     install_dir : ntpservicelistdir)
        install_data('org.freedesktop.ntstimesync1.policy',
                     install_dir : polkitpolicydir)
