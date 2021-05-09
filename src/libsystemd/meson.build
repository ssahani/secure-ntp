# SPDX-License-Identifier: LGPL-2.1-or-later

id128_sources = files('''
        sd-id128/id128-util.c
        sd-id128/id128-util.h
        sd-id128/sd-id128.c
'''.split())

############################################################

sd_daemon_sources = files('sd-daemon/sd-daemon.c')

############################################################

sd_event_sources = files('''
        sd-event/event-source.h
        sd-event/event-util.c
        sd-event/event-util.h
        sd-event/sd-event.c
'''.split())

############################################################

sd_login_sources = files('sd-login/sd-login.c')

############################################################

libsystemd_sources = files('''
        sd-network/network-util.c
        sd-network/network-util.h
        sd-network/sd-network.c
        sd-resolve/resolve-private.h
        sd-resolve/sd-resolve.c
'''.split()) + sd_daemon_sources + sd_event_sources + id128_sources + sd_login_sources

disable_mempool_c = files('disable-mempool.c')

libsystemd_c_args = ['-fvisibility=default']

libsystemd_static = static_library(
        'systemd_static',
        libsystemd_sources,
        install : false,
        include_directories : libsystemd_includes,
        link_with : libbasic,
        dependencies : [threads,
                        librt],
        c_args : libsystemd_c_args)

libsystemd_sym = files('libsystemd.sym')
libsystemd_sym_path = join_paths(meson.current_source_dir(), 'libsystemd.sym')

static_libsystemd = get_option('static-libsystemd')
static_libsystemd_pic = static_libsystemd == 'true' or static_libsystemd == 'pic'

configure_file(
        input : 'libsystemd.pc.in',
        output : 'libsystemd.pc',
        configuration : substs,
        install_dir : pkgconfiglibdir == 'no' ? '' : pkgconfiglibdir)

############################################################
