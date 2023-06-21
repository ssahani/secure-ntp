# SPDX-License-Identifier: LGPL-2.1-or-later

project('nts-timesync', 'c',
        version : '0.1',
        license : 'LGPLv2+',
        default_options: [
                'c_std=gnu99',
                'prefix=/usr',
                'sysconfdir=/etc',
                'localstatedir=/var',
                'warning_level=2',
        ],
        meson_version : '>= 0.46',
       )

libsystemd_version = '0.31.0'
libudev_version = '1.7.1'

# We need the same data in two different formats, ugh!
# Also, for hysterical reasons, we use different variable
# names, sometimes. Not all variables are included in every
# set. Ugh, ugh, ugh!
conf = configuration_data()
conf.set('PROJECT_VERSION',        meson.project_version(),
         description : 'Numerical project version (used where a simple number is expected)')

substs = configuration_data()
substs.set('PROJECT_URL',          'https://www.freedesktop.org/wiki/Software/systemd')
substs.set('PROJECT_VERSION',      meson.project_version(),
           description : 'Numerical project version (used where a simple number is expected)')

# This is to be used instead of meson.source_root(), as the latter will return
# the wrong result when systemd is being built as a meson subproject
project_source_root = meson.current_source_dir()
project_build_root = meson.current_build_dir()
relative_source_path = run_command('realpath',
                                   '--relative-to=@0@'.format(project_build_root),
                                   project_source_root).stdout().strip()
conf.set_quoted('RELATIVE_SOURCE_PATH', relative_source_path)

conf.set10('BUILD_MODE_DEVELOPER', get_option('mode') == 'developer',
           description : 'tailor build to development or release builds')


#####################################################################

if get_option('split-usr') == 'auto'
        split_usr = run_command('test', '-L', '/bin').returncode() != 0
else
        split_usr = get_option('split-usr') == 'true'
endif
if split_usr
        warning('\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n'
                + '                    split-usr mode is going to be removed\n' +
                '\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
endif
conf.set10('HAVE_SPLIT_USR', split_usr,
           description : '/usr/bin and /bin directories are separate')

if get_option('split-bin') == 'auto'
        split_bin = run_command('test', '-L', '/usr/sbin').returncode() != 0
else
        split_bin = get_option('split-bin') == 'true'
endif
conf.set10('HAVE_SPLIT_BIN', split_bin,
           description : 'bin and sbin directories are separate')

rootprefixdir = get_option('rootprefix')
# Unusual rootprefixdir values are used by some distros
# (see https://github.com/systemd/systemd/pull/7461).
rootprefix_default = split_usr ? '/' : '/usr'
if rootprefixdir == ''
        rootprefixdir = rootprefix_default
endif
rootprefixdir_noslash = rootprefixdir == '/' ? '' : rootprefixdir

have_standalone_binaries = get_option('standalone-binaries')

conf.set10('BUMP_PROC_SYS_FS_FILE_MAX', get_option('bump-proc-sys-fs-file-max'))
conf.set10('BUMP_PROC_SYS_FS_NR_OPEN',  get_option('bump-proc-sys-fs-nr-open'))
conf.set('HIGH_RLIMIT_NOFILE',          512*1024)

# join_paths ignores the preceding arguments if an absolute component is
# encountered, so this should canonicalize various paths when they are
# absolute or relative.
prefixdir = get_option('prefix')
if not prefixdir.startswith('/')
        error('Prefix is not absolute: "@0@"'.format(prefixdir))
endif
if prefixdir != rootprefixdir and not prefixdir.startswith(rootprefixdir.strip('/') + '/')
        error('Prefix is not below root prefix (now rootprefix=@0@ prefix=@1@)'.format(
                rootprefixdir, prefixdir))
endif

bindir = join_paths(prefixdir, get_option('bindir'))
libdir = join_paths(prefixdir, get_option('libdir'))
sysconfdir = join_paths(prefixdir, get_option('sysconfdir'))
includedir = join_paths(prefixdir, get_option('includedir'))
datadir = join_paths(prefixdir, get_option('datadir'))
localstatedir = join_paths('/', get_option('localstatedir'))

rootbindir = join_paths(rootprefixdir, 'bin')
rootsbindir = join_paths(rootprefixdir, split_bin ? 'sbin' : 'bin')
rootlibexecdir = join_paths(rootprefixdir, 'lib/systemd')

rootlibdir = get_option('rootlibdir')
if rootlibdir == ''
        rootlibdir = join_paths(rootprefixdir, libdir.split('/')[-1])
endif

install_sysconfdir = get_option('install-sysconfdir') != 'false'
install_sysconfdir_samples = get_option('install-sysconfdir') == 'true'
# Dirs of external packages
pkgconfigdatadir = get_option('pkgconfigdatadir') == '' ? join_paths(datadir, 'pkgconfig') : get_option('pkgconfigdatadir')
pkgconfiglibdir = get_option('pkgconfiglibdir') == '' ? join_paths(libdir, 'pkgconfig') : get_option('pkgconfiglibdir')
polkitpolicydir = join_paths(datadir, 'polkit-1/actions')
polkitrulesdir = join_paths(datadir, 'polkit-1/rules.d')
polkitpkladir = join_paths(localstatedir, 'lib/polkit-1/localauthority/10-vendor.d')
xinitrcdir = get_option('xinitrcdir') == '' ? join_paths(sysconfdir, 'X11/xinit/xinitrc.d') : get_option('xinitrcdir')

# Our own paths
pkgdatadir = join_paths(datadir, 'systemd')
environmentdir = join_paths(prefixdir, 'lib/environment.d')
pkgsysconfdir = join_paths(sysconfdir, 'nts-timesync')
userunitdir = join_paths(prefixdir, 'lib/systemd/user')
userpresetdir = join_paths(prefixdir, 'lib/systemd/user-preset')
tmpfilesdir = join_paths(prefixdir, 'lib/tmpfiles.d')
sysusersdir = join_paths(prefixdir, 'lib/sysusers.d')
sysctldir = join_paths(prefixdir, 'lib/sysctl.d')
binfmtdir = join_paths(prefixdir, 'lib/binfmt.d')
modulesloaddir = join_paths(prefixdir, 'lib/modules-load.d')
networkdir = join_paths(rootprefixdir, 'lib/systemd/network')
pkgincludedir = join_paths(includedir, 'systemd')
systemgeneratordir = join_paths(rootlibexecdir, 'system-generators')
systemunitdir = join_paths(rootprefixdir, 'lib/systemd/system')
systempresetdir = join_paths(rootprefixdir, 'lib/systemd/system-preset')
factorydir = join_paths(datadir, 'factory')
bootlibdir = join_paths(prefixdir, 'lib/systemd/boot/efi')
testsdir = join_paths(prefixdir, 'lib/systemd/tests')
systemdstatedir = join_paths(localstatedir, 'lib/systemd')
catalogstatedir = join_paths(systemdstatedir, 'catalog')
randomseeddir = join_paths(localstatedir, 'lib/systemd')
profiledir = join_paths(rootlibexecdir, 'portable', 'profile')
ntpservicelistdir = join_paths(rootprefixdir, 'lib/systemd/ntp-units.d')

docdir = get_option('docdir')
if docdir == ''
        docdir = join_paths(datadir, 'doc/systemd')
endif

dbuspolicydir = get_option('dbuspolicydir')
if dbuspolicydir == ''
        dbuspolicydir = join_paths(datadir, 'dbus-1/system.d')
endif

dbussessionservicedir = get_option('dbussessionservicedir')
if dbussessionservicedir == ''
        dbussessionservicedir = join_paths(datadir, 'dbus-1/services')
endif

dbussystemservicedir = get_option('dbussystemservicedir')
if dbussystemservicedir == ''
        dbussystemservicedir = join_paths(datadir, 'dbus-1/system-services')
endif

status_unit_format_default = get_option('status-unit-format-default')

conf.set_quoted('PKGSYSCONFDIR',                              pkgsysconfdir)
conf.set_quoted('SYSTEM_CONFIG_UNIT_DIR',                     join_paths(pkgsysconfdir, 'system'))
conf.set_quoted('SYSTEM_DATA_UNIT_PATH',                      systemunitdir)

conf.set('ANSI_OK_COLOR',                                     'ANSI_' + get_option('ok-color').underscorify().to_upper())
conf.set10('ENABLE_FEXECVE',                                  get_option('fexecve'))

conf.set_quoted('USER_CONFIG_UNIT_DIR',                       join_paths(pkgsysconfdir, 'user'))
conf.set_quoted('CATALOG_DATABASE',                           join_paths(catalogstatedir, 'database'))
conf.set_quoted('SYSTEMD_BINARY_PATH',                        join_paths(rootlibexecdir, 'systemd'))
conf.set_quoted('SYSTEMD_CGROUPS_AGENT_PATH',                 join_paths(rootlibexecdir, 'systemd-cgroups-agent'))
conf.set_quoted('SYSTEMD_FSCK_PATH',                          join_paths(rootlibexecdir, 'systemd-fsck'))
conf.set_quoted('SYSTEMCTL_BINARY_PATH',                      join_paths(rootbindir, 'systemctl'))
conf.set_quoted('SYSTEMD_TTY_ASK_PASSWORD_AGENT_BINARY_PATH', join_paths(rootbindir, 'systemd-tty-ask-password-agent'))
conf.set_quoted('SYSTEMD_STDIO_BRIDGE_BINARY_PATH',           join_paths(bindir, 'systemd-stdio-bridge'))
conf.set_quoted('ROOTPREFIX',                                 rootprefixdir)
conf.set_quoted('ROOTPREFIX_NOSLASH',                         rootprefixdir_noslash)
conf.set_quoted('RANDOM_SEED_DIR',                            randomseeddir)
conf.set_quoted('SYSTEMD_TEST_DATA',                          join_paths(testsdir, 'testdata'))
conf.set_quoted('POLKIT_AGENT_BINARY_PATH',                   join_paths(bindir, 'pkttyagent'))
conf.set_quoted('LIBDIR',                                     libdir)
conf.set_quoted('ROOTLIBDIR',                                 rootlibdir)
conf.set_quoted('ROOTLIBEXECDIR',                             rootlibexecdir)
conf.set_quoted('DOCUMENT_ROOT',                              join_paths(pkgdatadir, 'gatewayd'))
conf.set('STATUS_UNIT_FORMAT_DEFAULT',                        'STATUS_UNIT_FORMAT_' + status_unit_format_default.to_upper())

substs.set('prefix',                                          prefixdir)
substs.set('rootprefix',                                      rootprefixdir)
substs.set('rootprefix_noslash',                              rootprefixdir_noslash)
substs.set('exec_prefix',                                     prefixdir)
substs.set('libdir',                                          libdir)
substs.set('rootlibdir',                                      rootlibdir)
substs.set('includedir',                                      includedir)
substs.set('sysconfdir',                                      sysconfdir)
substs.set('bindir',                                          bindir)
substs.set('rootbindir',                                      rootbindir)
substs.set('rootlibexecdir',                                  rootlibexecdir)
substs.set('systemunitdir',                                   systemunitdir)
substs.set('userunitdir',                                     userunitdir)
substs.set('systempresetdir',                                 systempresetdir)
substs.set('userpresetdir',                                   userpresetdir)
substs.set('environmentdir',                                  environmentdir)
substs.set('RANDOM_SEED',                                     join_paths(randomseeddir, 'random-seed'))
substs.set('SYSTEMD_TEST_DATA',                               join_paths(testsdir, 'testdata'))
substs.set('STATUS_UNIT_FORMAT_DEFAULT',                      status_unit_format_default)
substs.set('HIGH_RLIMIT_NOFILE',                              conf.get('HIGH_RLIMIT_NOFILE'))
substs.set('BUILD_ROOT',                                      project_build_root)

#####################################################################

cc = meson.get_compiler('c')
pkgconfig = import('pkgconfig')
check_compilation_sh = find_program('tools/check-compilation.sh')
meson_build_sh = find_program('tools/meson-build.sh')

want_tests = get_option('tests')
slow_tests = want_tests != 'false' and get_option('slow-tests')
fuzz_tests = want_tests != 'false' and get_option('fuzz-tests')
install_tests = get_option('install-tests')

# Those generate many false positives, and we do not want to change the code to
# avoid them.
basic_disabled_warnings = [
        '-Wno-unused-parameter',
        '-Wno-missing-field-initializers',
        '-Wno-unused-result',
        '-Wno-format-signedness',
]

possible_cc_flags = [
        '-Werror=undef',
        '-Wlogical-op',
        '-Wmissing-include-dirs',
        '-Wold-style-definition',
        '-Wpointer-arith',
        '-Winit-self',
        '-Wfloat-equal',
        '-Wsuggest-attribute=noreturn',
        '-Werror=missing-prototypes',
        '-Werror=implicit-function-declaration',
        '-Werror=missing-declarations',
        '-Werror=return-type',
        '-Werror=incompatible-pointer-types',
        '-Werror=format=2',
        '-Wstrict-prototypes',
        '-Wredundant-decls',
        '-Wmissing-noreturn',
        '-Wimplicit-fallthrough=5',
        '-Wshadow',
        '-Wendif-labels',
        '-Wstrict-aliasing=2',
        '-Wwrite-strings',
        '-Werror=overflow',
        '-Werror=shift-count-overflow',
        '-Werror=shift-overflow=2',
        '-Wdate-time',
        '-Wnested-externs',

        # negative arguments are correctly detected starting with meson 0.46.
        '-Wno-error=#warnings',  # clang
        '-Wno-string-plus-int',  # clang

        '-ffast-math',
        '-fno-common',
        '-fdiagnostics-show-option',
        '-fno-strict-aliasing',
        '-fvisibility=hidden',
        '-fstack-protector',
        '-fstack-protector-strong',
        '--param=ssp-buffer-size=4',
]

# Disable -Wmaybe-unitialized when compiling with -Os/-O1/-O3/etc. There are
# too many false positives with gcc >= 8. Effectively, we only test with -O0
# and -O2; this should be enough to catch most important cases without too much
# busywork. See https://github.com/systemd/systemd/pull/19226.
if cc.get_id() == 'gcc' and (not '02'.contains(get_option('optimization')) or
                             cc.version().version_compare('<10'))
        possible_cc_flags += '-Wno-maybe-uninitialized'
endif

# --as-needed and --no-undefined are provided by meson by default,
# run mesonconf to see what is enabled
possible_link_flags = [
        '-Wl,-z,relro',
        '-Wl,-z,now',
        '-fstack-protector',
]

if cc.get_id() == 'clang'
        possible_cc_flags += [
                '-Wno-typedef-redefinition',
                '-Wno-gnu-variable-sized-type-not-at-end',
        ]
endif

if get_option('buildtype') != 'debug'
        possible_cc_flags += [
                '-ffunction-sections',
                '-fdata-sections',
        ]

        possible_link_flags += '-Wl,--gc-sections'
endif

add_project_arguments(cc.get_supported_arguments(basic_disabled_warnings), language : 'c')
add_project_arguments(cc.get_supported_arguments(possible_cc_flags), language : 'c')
add_project_link_arguments(cc.get_supported_link_arguments(possible_link_flags), language : 'c')

have = cc.has_argument('-Wzero-length-bounds')
conf.set10('HAVE_ZERO_LENGTH_BOUNDS', have)

if cc.compiles('''
   #include <time.h>
   #include <inttypes.h>
   typedef uint64_t usec_t;
   usec_t now(clockid_t clock);
   int main(void) {
           struct timespec now;
           return 0;
   }
''', args: '-Werror=shadow', name : '-Werror=shadow with local shadowing')
        add_project_arguments('-Werror=shadow', language : 'c')
endif

cpp = ' '.join(cc.cmd_array()) + ' -E'

has_wstringop_truncation = cc.has_argument('-Wstringop-truncation')

#####################################################################
# compilation result tests

conf.set('_GNU_SOURCE', true)
conf.set('__SANE_USERSPACE_TYPES__', true)
conf.set10('HAVE_WSTRINGOP_TRUNCATION', has_wstringop_truncation)

conf.set('SIZEOF_DEV_T', cc.sizeof('dev_t', prefix : '#include <sys/types.h>'))
conf.set('SIZEOF_INO_T', cc.sizeof('ino_t', prefix : '#include <sys/types.h>'))
conf.set('SIZEOF_TIME_T', cc.sizeof('time_t', prefix : '#include <sys/time.h>'))
conf.set('SIZEOF_RLIM_T', cc.sizeof('rlim_t', prefix : '#include <sys/resource.h>'))

decl_headers = '''
#include <uchar.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <linux/fs.h>
'''

foreach decl : ['char16_t',
                'char32_t',
                'struct mount_attr',
                'struct statx',
               ]

        # We get -1 if the size cannot be determined
        have = cc.sizeof(decl, prefix : decl_headers, args : '-D_GNU_SOURCE') > 0

        if decl == 'struct statx'
                if have
                        want_linux_stat_h = false
                else
                        have = cc.sizeof(decl,
                                         prefix : decl_headers + '#include <linux/stat.h>',
                                         args : '-D_GNU_SOURCE') > 0
                        want_linux_stat_h = have
                endif
        endif

        conf.set10('HAVE_' + decl.underscorify().to_upper(), have)
endforeach

conf.set10('WANT_LINUX_STAT_H', want_linux_stat_h)

foreach ident : ['secure_getenv', '__secure_getenv']
        conf.set10('HAVE_' + ident.to_upper(), cc.has_function(ident))
endforeach

foreach ident : [
        ['memfd_create',      '''#include <sys/mman.h>'''],
        ['gettid',            '''#include <sys/types.h>
                                 #include <unistd.h>'''],
        ['pivot_root',        '''#include <stdlib.h>
                                 #include <unistd.h>'''],     # no known header declares pivot_root
        ['name_to_handle_at', '''#include <sys/types.h>
                                 #include <sys/stat.h>
                                 #include <fcntl.h>'''],
        ['setns',             '''#include <sched.h>'''],
        ['renameat2',         '''#include <stdio.h>
                                 #include <fcntl.h>'''],
        ['kcmp',              '''#include <linux/kcmp.h>'''],
        ['keyctl',            '''#include <sys/types.h>
                                 #include <keyutils.h>'''],
        ['copy_file_range',   '''#include <sys/syscall.h>
                                 #include <unistd.h>'''],
        ['bpf',               '''#include <sys/syscall.h>
                                 #include <unistd.h>'''],
        ['statx',             '''#include <sys/types.h>
                                 #include <sys/stat.h>
                                 #include <unistd.h>'''],
        ['explicit_bzero' ,   '''#include <string.h>'''],
        ['reallocarray',      '''#include <stdlib.h>'''],
        ['set_mempolicy',     '''#include <stdlib.h>
                                 #include <unistd.h>'''],
        ['get_mempolicy',     '''#include <stdlib.h>
                                 #include <unistd.h>'''],
        ['pidfd_send_signal', '''#include <stdlib.h>
                                 #include <unistd.h>
                                 #include <signal.h>
                                 #include <sys/wait.h>'''],
        ['pidfd_open',        '''#include <stdlib.h>
                                 #include <unistd.h>
                                 #include <signal.h>
                                 #include <sys/wait.h>'''],
        ['rt_sigqueueinfo',   '''#include <stdlib.h>
                                 #include <unistd.h>
                                 #include <signal.h>
                                 #include <sys/wait.h>'''],
        ['mallinfo',          '''#include <malloc.h>'''],
        ['mallinfo2',         '''#include <malloc.h>'''],
        ['execveat',          '''#include <unistd.h>'''],
        ['close_range',       '''#include <unistd.h>'''],
        ['epoll_pwait2',      '''#include <sys/epoll.h>'''],
        ['mount_setattr',     '''#include <sys/mount.h>'''],
        ['move_mount',        '''#include <sys/mount.h>'''],
        ['open_tree',         '''#include <sys/mount.h>'''],
]

        have = cc.has_function(ident[0], prefix : ident[1], args : '-D_GNU_SOURCE')
        conf.set10('HAVE_' + ident[0].to_upper(), have)
endforeach

if cc.has_function('getrandom', prefix : '''#include <sys/random.h>''', args : '-D_GNU_SOURCE')
        conf.set10('USE_SYS_RANDOM_H', true)
        conf.set10('HAVE_GETRANDOM', true)
else
        have = cc.has_function('getrandom', prefix : '''#include <linux/random.h>''')
        conf.set10('USE_SYS_RANDOM_H', false)
        conf.set10('HAVE_GETRANDOM', have)
endif

#####################################################################

version_tag = get_option('version-tag')
if version_tag != ''
        vcs_data = configuration_data()
        vcs_data.set('VCS_TAG', version_tag)
        version_h = configure_file(configuration : vcs_data,
                                   input : 'src/version/version.h.in',
                                   output : 'version.h')
else
        vcs_tagger = [
                project_source_root + '/tools/meson-vcs-tag.sh',
                project_source_root,
                meson.project_version()]

        version_h = vcs_tag(
                input : 'src/version/version.h.in',
                output : 'version.h',
                command: vcs_tagger)
endif

versiondep = declare_dependency(sources: version_h)

sed = find_program('sed')
awk = find_program('awk')
m4 = find_program('m4')
stat = find_program('stat')
git = find_program('git', required : false)
env = find_program('env')
perl = find_program('perl', required : false)

meson_make_symlink = project_source_root + '/tools/meson-make-symlink.sh'
mkdir_p = 'mkdir -p $DESTDIR/@0@'

# if -Dxxx-path option is found, use that. Otherwise, check in $PATH,
# /usr/sbin, /sbin, and fall back to the default from middle column.
progs = [['quotaon',    '/usr/sbin/quotaon'    ],
         ['quotacheck', '/usr/sbin/quotacheck' ],
         ['sulogin',    '/usr/sbin/sulogin'    ],
         ['nologin',    '/usr/sbin/nologin',   ],
        ]
foreach prog : progs
        path = get_option(prog[0] + '-path')
        if path != ''
                message('Using @1@ for @0@'.format(prog[0], path))
        else
                exe = find_program(prog[0],
                                   '/usr/sbin/' + prog[0],
                                   '/sbin/' + prog[0],
                                   required: false)
                path = exe.found() ? exe.path() : prog[1]
        endif
        name = prog.length() > 2 ? prog[2] : prog[0].to_upper()
        conf.set_quoted(name, path)
        substs.set(name, path)
endforeach

if run_command('ln', '--relative', '--help').returncode() != 0
        error('ln does not support --relative (added in coreutils 8.16)')
endif

############################################################

gperf = find_program('gperf')

gperf_test_format = '''
#include <string.h>
const char * in_word_set(const char *, @0@);
@1@
'''
gperf_snippet_format = 'echo foo,bar | @0@ -L ANSI-C'
gperf_snippet = run_command('sh', '-c', gperf_snippet_format.format(gperf.path()))
gperf_test = gperf_test_format.format('size_t', gperf_snippet.stdout())
if cc.compiles(gperf_test)
        gperf_len_type = 'size_t'
else
        gperf_test = gperf_test_format.format('unsigned', gperf_snippet.stdout())
        if cc.compiles(gperf_test)
                gperf_len_type = 'unsigned'
        else
                error('unable to determine gperf len type')
        endif
endif
message('gperf len type is @0@'.format(gperf_len_type))
conf.set('GPERF_LEN_TYPE', gperf_len_type,
         description : 'The type of gperf "len" parameter')

############################################################

if not cc.has_header('sys/capability.h')
        error('POSIX caps headers not found')
endif
foreach header : ['crypt.h',
                  'linux/memfd.h',
                  'linux/vm_sockets.h',
                  'sys/auxv.h',
                  'valgrind/memcheck.h',
                  'valgrind/valgrind.h',
                  'linux/time_types.h',
                 ]

        conf.set10('HAVE_' + header.underscorify().to_upper(),
                   cc.has_header(header))
endforeach

############################################################

fallback_hostname = get_option('fallback-hostname')
if fallback_hostname == '' or fallback_hostname[0] == '.' or fallback_hostname[0] == '-'
        error('Invalid fallback-hostname configuration')
        # A more extensive test is done in test-hostname-util. Let's catch
        # the most obvious errors here so we don't fail with an assert later.
endif
conf.set_quoted('FALLBACK_HOSTNAME', fallback_hostname)

time_epoch = get_option('time-epoch')
if time_epoch == -1
        time_epoch = run_command('sh', ['-c', 'echo "$SOURCE_DATE_EPOCH"']).stdout().strip()
        if time_epoch == '' and git.found() and run_command('test', '-e', '.git').returncode() == 0
                # If we're in a git repository, use the creation time of the latest git tag.
                latest_tag = run_command('git', 'describe', '--abbrev=0', '--tags').stdout().strip()
                time_epoch = run_command('git', 'log', '--no-show-signature', '-1', '--format=%at', latest_tag).stdout()
        endif
        if time_epoch == ''
                NEWS = files('NEWS')
                time_epoch = run_command(stat, '-c', '%Y', NEWS).stdout()
        endif
        time_epoch = time_epoch.to_int()
endif
conf.set('TIME_EPOCH', time_epoch)

foreach tuple : [['system-alloc-uid-min', 'SYS_UID_MIN', 1],  # Also see login.defs(5).
                 ['system-uid-max',       'SYS_UID_MAX', 999],
                 ['system-alloc-gid-min', 'SYS_GID_MIN', 1],
                 ['system-gid-max',       'SYS_GID_MAX', 999]]
        v = get_option(tuple[0])
        if v == -1
                v = run_command(
                        awk,
                        '/^\s*@0@\s+/ { uid=$2 } END { print uid }'.format(tuple[1]),
                        '/etc/login.defs').stdout().strip()
                if v == ''
                        v = tuple[2]
                else
                        v = v.to_int()
                endif
        endif
        conf.set(tuple[0].underscorify().to_upper(), v)
        substs.set(tuple[0].underscorify().to_upper(), v)
endforeach
if conf.get('SYSTEM_ALLOC_UID_MIN') >= conf.get('SYSTEM_UID_MAX')
        error('Invalid uid allocation range')
endif
if conf.get('SYSTEM_ALLOC_GID_MIN') >= conf.get('SYSTEM_GID_MAX')
        error('Invalid gid allocation range')
endif

dynamic_uid_min = get_option('dynamic-uid-min')
dynamic_uid_max = get_option('dynamic-uid-max')
conf.set('DYNAMIC_UID_MIN', dynamic_uid_min)
conf.set('DYNAMIC_UID_MAX', dynamic_uid_max)
substs.set('dynamicuidmin', dynamic_uid_min)
substs.set('dynamicuidmax', dynamic_uid_max)

container_uid_base_min = get_option('container-uid-base-min')
container_uid_base_max = get_option('container-uid-base-max')
conf.set('CONTAINER_UID_BASE_MIN', container_uid_base_min)
conf.set('CONTAINER_UID_BASE_MAX', container_uid_base_max)
substs.set('containeruidbasemin', container_uid_base_min)
substs.set('containeruidbasemax', container_uid_base_max)

nobody_user = get_option('nobody-user')
nobody_group = get_option('nobody-group')

if not meson.is_cross_build()
        getent_result = run_command('getent', 'passwd', '65534')
        if getent_result.returncode() == 0
                name = getent_result.stdout().split(':')[0]
                if name != nobody_user
                        warning('\n' +
                                'The local user with the UID 65534 does not match the configured user name "@0@" of the nobody user (its name is @1@).\n'.format(nobody_user, name) +
                                'Your build will result in an user table setup that is incompatible with the local system.')
                endif
        endif
        id_result = run_command('id', '-u', nobody_user)
        if id_result.returncode() == 0
                id = id_result.stdout().to_int()
                if id != 65534
                        warning('\n' +
                                'The local user with the configured user name "@0@" of the nobody user does not have UID 65534 (it has @1@).\n'.format(nobody_user, id) +
                                'Your build will result in an user table setup that is incompatible with the local system.')
                endif
        endif

        getent_result = run_command('getent', 'group', '65534')
        if getent_result.returncode() == 0
                name = getent_result.stdout().split(':')[0]
                if name != nobody_group
                        warning('\n' +
                                'The local group with the GID 65534 does not match the configured group name "@0@" of the nobody group (its name is @1@).\n'.format(nobody_group, name) +
                                'Your build will result in an group table setup that is incompatible with the local system.')
                endif
        endif
        id_result = run_command('id', '-g', nobody_group)
        if id_result.returncode() == 0
                id = id_result.stdout().to_int()
                if id != 65534
                        warning('\n' +
                                'The local group with the configured group name "@0@" of the nobody group does not have GID 65534 (it has @1@).\n'.format(nobody_group, id) +
                                'Your build will result in an group table setup that is incompatible with the local system.')
                endif
        endif
endif
if nobody_user != nobody_group and not (nobody_user == 'nobody' and nobody_group == 'nogroup')
        warning('\n' +
                'The configured user name "@0@" and group name "@0@" of the nobody user/group are not equivalent.\n'.format(nobody_user, nobody_group) +
                'Please re-check that both "nobody-user" and "nobody-group" options are correctly set.')
endif

conf.set_quoted('NOBODY_USER_NAME', nobody_user)
conf.set_quoted('NOBODY_GROUP_NAME', nobody_group)
substs.set('NOBODY_USER_NAME', nobody_user)
substs.set('NOBODY_GROUP_NAME', nobody_group)

tty_gid = get_option('tty-gid')
conf.set('TTY_GID', tty_gid)
substs.set('TTY_GID', tty_gid)

# Ensure provided GID argument is numeric, otherwise fall back to default assignment
users_gid = get_option('users-gid')
substs.set('USERS_GID', users_gid < 0 ? '-' : users_gid)

conf.set10('ENABLE_ADM_GROUP', get_option('adm-group'))
conf.set10('ENABLE_WHEEL_GROUP', get_option('wheel-group'))

ntp_servers = get_option('ntp-servers')
conf.set_quoted('NTP_SERVERS', ntp_servers)
substs.set('NTP_SERVERS', ntp_servers)

localegen_path = get_option('localegen-path')
have = false
writable = ''
if localegen_path != ''
        conf.set_quoted('LOCALEGEN_PATH', localegen_path)
        have = true
        writable = ' /usr/lib/locale'
endif
substs.set('SERVICE_LOCALEGEN_WRITABLE', writable)
conf.set10('HAVE_LOCALEGEN', have)

conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())

service_watchdog = get_option('service-watchdog')
watchdog_value = service_watchdog == '' ? '' : 'WatchdogSec=' + service_watchdog
substs.set('SERVICE_WATCHDOG', watchdog_value)

conf.set10('LOG_TRACE', get_option('log-trace'))

enable_debug_hashmap = false
enable_debug_mmap_cache = false
enable_debug_siphash = false
foreach name : get_option('debug-extra')
        if name == 'hashmap'
                enable_debug_hashmap = true
        elif name == 'mmap-cache'
                enable_debug_mmap_cache = true
        elif name == 'siphash'
                enable_debug_siphash = true
        else
                message('unknown debug option "@0@", ignoring'.format(name))
        endif
endforeach
conf.set10('ENABLE_DEBUG_HASHMAP', enable_debug_hashmap)
conf.set10('ENABLE_DEBUG_MMAP_CACHE', enable_debug_mmap_cache)
conf.set10('ENABLE_DEBUG_SIPHASH', enable_debug_siphash)

conf.set10('VALGRIND', get_option('valgrind'))
conf.set10('LOG_TRACE', get_option('log-trace'))

default_user_path = get_option('user-path')
if default_user_path != ''
        conf.set_quoted('DEFAULT_USER_PATH', default_user_path)
        default_user_path_display = default_user_path
else
        # meson 0.49 fails when ?: is used in .format()
        default_user_path_display = '(same as system services)'
endif


#####################################################################

threads = dependency('threads')
librt = cc.find_library('rt')
libm = cc.find_library('m')
libdl = cc.find_library('dl')
libcrypt = cc.find_library('crypt')

crypt_header = conf.get('HAVE_CRYPT_H') == 1 ? '''#include <crypt.h>''' : '''#include <unistd.h>'''
foreach ident : [
        ['crypt_ra',               crypt_header],
        ['crypt_preferred_method', crypt_header],
        ['crypt_gensalt_ra',       crypt_header]]

        have = cc.has_function(ident[0], prefix : ident[1], args : '-D_GNU_SOURCE',
                               dependencies : libcrypt)
        conf.set10('HAVE_' + ident[0].to_upper(), have)
endforeach

libcap = dependency('libcap', required : false)
if not libcap.found()
        # Compat with Ubuntu 14.04 which ships libcap w/o .pc file
        libcap = cc.find_library('cap')
endif

want_audit = get_option('audit')
if want_audit != 'false'
        libaudit = dependency('audit', required : want_audit == 'true')
        have = libaudit.found()
else
        have = false
        libaudit = []
endif
conf.set10('HAVE_AUDIT', have)

want_selinux = get_option('selinux')
if want_selinux != 'false'
        libselinux = dependency('libselinux',
                                version : '>= 2.1.9',
                                required : want_selinux == 'true')
        have = libselinux.found()
else
        have = false
        libselinux = []
endif
conf.set10('HAVE_SELINUX', have)

want_gnutls = get_option('gnutls')
if want_gnutls != 'false'
        libgnutls = dependency('gnutls',
                               version : '>= 3.1.4',
                               required : want_gnutls == 'true')
        have = libgnutls.found()
else
        have = false
        libgnutls = []
endif
conf.set10('HAVE_GNUTLS', have)

want_openssl = get_option('openssl')
if want_openssl != 'false'
        libopenssl = dependency('openssl',
                                version : '>= 1.1.0',
                                required : want_openssl == 'true')
        have = libopenssl.found()
else
        have = false
        libopenssl = []
endif
conf.set10('HAVE_OPENSSL', have)

want_glib = get_option('glib')
if want_glib != 'false'
        libglib =    dependency('glib-2.0',
                                version : '>= 2.22.0',
                                required : want_glib == 'true')
        libgobject = dependency('gobject-2.0',
                                version : '>= 2.22.0',
                                required : want_glib == 'true')
        libgio =     dependency('gio-2.0',
                                required : want_glib == 'true')
        have = libglib.found() and libgobject.found() and libgio.found()
else
        have = false
        libglib = []
        libgobject = []
        libgio = []
endif
conf.set10('HAVE_GLIB', have)

want_dbus = get_option('dbus')
if want_dbus != 'false'
        libdbus = dependency('dbus-1',
                             version : '>= 1.3.2',
                             required : want_dbus == 'true')
        have = libdbus.found()
else
        have = false
        libdbus = []
endif
conf.set10('HAVE_DBUS', have)

foreach term : ['timesyncd']
        have = get_option(term)
        name = 'ENABLE_' + term.underscorify().to_upper()
        conf.set10(name, have)
        substs.set10(name, have)
endforeach

#####################################################################
generate_gperfs = find_program('tools/generate-gperfs.py')
update_syscall_tables_sh = find_program('tools/update-syscall-tables.sh')
#####################################################################


config_h = configure_file(
        output : 'config.h',
        configuration : conf)

add_project_arguments('-include', 'config.h', language : 'c')

############################################################

# binaries that have --help and are intended for use by humans,
# usually, but not always, installed in /bin.
public_programs = []

tests = []
fuzzers = []

basic_includes = include_directories(
        'src/basic',
        'src/fundamental',
        'src/systemd',
        '.')

libsystemd_includes = [basic_includes, include_directories(
        'src/libsystemd/sd-network',
        'src/libsystemd/sd-resolve')]

includes = [libsystemd_includes, include_directories('src/shared')]

subdir('src/fundamental')
subdir('src/basic')
subdir('src/libsystemd')
subdir('src/shared')
subdir('units')

install_libsystemd_static = static_library(
        'systemd',
        libsystemd_sources,
        basic_sources,
        fundamental_sources,
        disable_mempool_c,
        include_directories : libsystemd_includes,
        build_by_default : static_libsystemd != 'false',
        install : static_libsystemd != 'false',
        install_dir : rootlibdir,
        pic : static_libsystemd_pic,
        dependencies : [threads,
                        librt,
                        libcap,
                        libselinux],
        c_args : libsystemd_c_args + (static_libsystemd_pic ? [] : ['-fno-PIC']))

############################################################

subdir('src/nts-timesync')

############################################################
executable(
                'nts-timesyncd',
                nts_timesyncd_sources,
                include_directories : includes,
                link_with : [lib_nts_timesyncd_core],
                dependencies : [threads,
                                libgnutls,
                                libm],
                install_rpath : bindir,
                install : true,
                install_dir : bindir)

############################################################
if git.found()
        all_files = run_command(
                'env', '-u', 'GIT_WORK_TREE',
                git,
                '--git-dir=@0@/.git'.format(project_source_root),
                'ls-files', ':/*.[ch]')

        all_files = files(all_files.stdout().split())

        custom_target(
                'tags',
                output : 'tags',
                command : [env, 'etags', '-o', '@0@/TAGS'.format(project_source_root)] + all_files)
        run_target(
                'ctags',
                command : [env, 'ctags', '-o', '@0@/tags'.format(project_source_root)] + all_files)
endif

if git.found()
        git_contrib_sh = find_program('tools/git-contrib.sh')
        run_target(
                'git-contrib',
                command : [git_contrib_sh])
endif

if git.found()
        git_head = run_command(
                git,
                ['--git-dir=@0@/.git'.format(project_source_root),
                 'rev-parse', 'HEAD']).stdout().strip()
        git_head_short = run_command(
                git,
                ['--git-dir=@0@/.git'.format(project_source_root),
                 'rev-parse', '--short=7', 'HEAD']).stdout().strip()

        run_target(
                'git-snapshot',
                command : ['git', 'archive',
                           '-o', '@0@/systemd-@1@.tar.gz'.format(project_source_root,
                                                                 git_head_short),
                           '--prefix', 'systemd-@0@/'.format(git_head),
                           'HEAD'])
endif

############################################################
watchdog_opt = service_watchdog == '' ? 'disabled' : service_watchdog

status = [
        '@0@ @1@'.format(meson.project_name(), meson.project_version()),

        'build mode:                        @0@'.format(get_option('mode')),
        'split /usr:                        @0@'.format(split_usr),
        'split bin-sbin:                    @0@'.format(split_bin),
        'prefix directory:                  @0@'.format(prefixdir),
        'rootprefix directory:              @0@'.format(rootprefixdir),
        'sysconf directory:                 @0@'.format(sysconfdir),
        'include directory:                 @0@'.format(includedir),
        'lib directory:                     @0@'.format(libdir),
        'rootlib directory:                 @0@'.format(rootlibdir),
        'D-Bus policy directory:            @0@'.format(dbuspolicydir),
        'D-Bus session directory:           @0@'.format(dbussessionservicedir),
        'D-Bus system directory:            @0@'.format(dbussystemservicedir),
        'TTY GID:                           @0@'.format(tty_gid),
        'users GID:                         @0@'.format(substs.get('USERS_GID')),
        'system UIDs:                       <=@0@ (alloc >=@1@)'.format(conf.get('SYSTEM_UID_MAX'),
                                                                        conf.get('SYSTEM_ALLOC_UID_MIN')),
        'system GIDs:                       <=@0@ (alloc >=@1@)'.format(conf.get('SYSTEM_GID_MAX'),
                                                                        conf.get('SYSTEM_ALLOC_GID_MIN')),
        'dynamic UIDs:                      @0@…@1@'.format(dynamic_uid_min, dynamic_uid_max),
        'container UID bases:               @0@…@1@'.format(container_uid_base_min, container_uid_base_max),
        'nobody user name:                  @0@'.format(nobody_user),
        'nobody group name:                 @0@'.format(nobody_group),
        'fallback hostname:                 @0@'.format(get_option('fallback-hostname')),
        'default user $PATH:                @0@'.format(default_user_path_display),
        'systemd service watchdog:          @0@'.format(watchdog_opt)]

alt_ntp_servers = '\n                                            '.join(ntp_servers.split(' '))
status += [
        'default NTP servers:               @0@'.format(alt_ntp_servers)]

alt_time_epoch = run_command('date', '-Is', '-u', '-d',
                             '@@0@'.format(time_epoch)).stdout().strip()
status += [
        'time epoch:                        @0@ (@1@)'.format(time_epoch, alt_time_epoch)]

# TODO:
# CFLAGS:   ${OUR_CFLAGS} ${CFLAGS}
# CPPFLAGS: ${OUR_CPPFLAGS} ${CPPFLAGS}
# LDFLAGS:  ${OUR_LDFLAGS} ${LDFLAGS}

found = []
missing = []

foreach tuple : [
        # dependencies
        ['ACL'],
        ['AUDIT'],
        ['SECCOMP'],
        ['SELinux'],
        ['gnutls'],
        ['openssl'],
        ['pcre2'],

        # components
        ['timesyncd'],

        # optional features
        ['polkit'],
        ['dbus'],
        ['glib'],
        ['compat-mutable-uid-boundaries'],
        ['link-timesyncd-shared', get_option('link-timesyncd-shared')],
        ['fexecve'],
        ['standalone-binaries',   get_option('standalone-binaries')],
]

        if tuple.length() >= 2
                cond = tuple[1]
        else
                ident1 = 'HAVE_' + tuple[0].underscorify().to_upper()
                ident2 = 'ENABLE_' + tuple[0].underscorify().to_upper()
                cond = conf.get(ident1, 0) == 1 or conf.get(ident2, 0) == 1
        endif
        if cond
                found += tuple[0]
        else
                missing += tuple[0]
        endif
endforeach

if static_libsystemd == 'false'
        missing += 'static-libsystemd'
else
        found += 'static-libsystemd(@0@)'.format(static_libsystemd)
endif

status += [
        '',
        'enabled features: @0@'.format(', '.join(found)),
        '',
        'disabled features: @0@'.format(', '.join(missing)),
        '']
message('\n         '.join(status))

if rootprefixdir != rootprefix_default
        warning('\n' +
                'Note that the installation prefix was changed to "@0@".\n'.format(rootprefixdir) +
                'systemd used fixed names for unit file directories and other paths, so anything\n' +
                'except the default ("@0@") is strongly discouraged.'.format(rootprefix_default))
endif
