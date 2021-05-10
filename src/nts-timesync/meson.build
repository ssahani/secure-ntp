# SPDX-License-Identifier: LGPL-2.1+

sources = files(
        'timesyncd-conf.c',
        'timesyncd-conf.h',
        'timesyncd-manager.c',
        'timesyncd-manager.h',
        'timesyncd-nts-client.c',
        'timesyncd-nts-client.h',
        'timesyncd-ntp-extension.c',
        'timesyncd-ntp-extension.h',
        'timesyncd-ntske-client.c',
        'timesyncd-ntske-client.h',
        'timesyncd-ntske-protocol.c',
        'timesyncd-ntske-protocol.h',
        'timesyncd-ntp-message.h',
        'timesyncd-server.c',
        'timesyncd-server.h')

nts_timesyncd_sources = files(
        'timesyncd.c')

sources += custom_target(
        'nts-timesyncd-gperf.c',
        input : 'nts-timesyncd-gperf.gperf',
        output : 'nts-timesyncd-gperf.c',
        command : [gperf, '@INPUT@', '--output-file', '@OUTPUT@'])

nts_timesyncd_link_with = [libsystemd_static,
                          libshared_static]

lib_nts_timesyncd_core = static_library(
                       'timesyncd-core',
                        sources,
                        include_directories : includes,
                        link_with : [nts_timesyncd_link_with])
