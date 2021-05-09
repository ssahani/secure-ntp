nts-timesyncd: Configures NTP using Key Establishment (NTS-KE) protocol.

Introduction
============

nts-timesyncd is the systemd project's "timesyncd", extracted out to be a
standalone daemon.  It allows to Computers use the Network Time Protocol (NTP) to synchronize
their system clocks over the internet. NTP is one of the few unsecured internet
protocols still in common use. An attacker that can observe network traffic between
a client and server can feed the client with bogus data and, depending on the client’s
implementation and configuration, force it to set its system clock to any time and date.

NTS includes a Key Establishment (NTS-KE) protocol that automatically creates the encryption
keys used between the server and its clients. It uses Transport Layer Security (TLS) on TCP
port 4460. It is designed to scale to very large numbers of clients with a minimal impact on
accuracy. The server does not need to keep any client-specific state. It provides clients with
cookies, which are encrypted and contain the keys needed to authenticate the NTP packets.
Privacy is one of the goals of NTS. The client gets a new cookie with each server response,
so it doesn’t have to reuse cookies. This prevents passive observers from tracking clients migrating between networks.


All of the credit for etimesyncd should go to the systemd developers.
For more on systemd, see
  http://www.freedesktop.org/wiki/Software/systemd
All of the blame should go to Andy Wingo, who extracted etimesyncd
from systemd.
All complaints should go to Sven Eden, who is maintaining etimesyncd.

Contributing
============

nts-timesyncd was branched from systemd version 248. It implements
Network Time Security for the Network Time Protocol.
RFC: https://datatracker.ietf.org/doc/html/rfc8915 using GnuTls
https://www.gnutls.org/

To contribute to nts-timesyncd, fork the current source code from github:
  https://github.com/nts-timesyncd/nts-timesyncd
Send a pull request for the changes you like.

If you do not have a github account, the nts-timesyncd wiki page at
  https://github.com/nts-timesyncd/nts-timesyncd/wiki
lists further possibilities to contact the maintainers.

A forum subsection has been set up for nts-timesyncd at
  https://forums.prydeworx.com
where you can register/login with your GitLab/GitHub account.

Finally, bug reports:
  https://github.com/nts-timesyncd/nts-timesyncd/issues

License
=======

LGPLv2.1+ for all code
  - except `src/basic/MurmurHash2.c` which is Public Domain
  - except `src/basic/siphash24.c` which is CC0 Public Domain

Dependencies
============

  * glibc >= 2.16 (*or* musl-libc >= 1.1.20)
  * libcap
  * libacl (optional)
  * libselinux (optional)
  * libaudit (optional)
  * GnuTLS
  * pkg-config
  * gperf >= 3.1
  * python, meson, ninja
  * gcc, awk, sed, grep, m4, and similar tools

During runtime, you need the following additional dependencies:
---------------------------------------------------------------
  * util-linux >= v2.27.1 required
  * dbus >= 1.9.14 (strictly speaking optional, but recommended)
    NOTE: If using dbus < 1.9.18, you should override the default
          policy directory (--with-dbuspolicydir=/etc/dbus-1/system.d).
  * PolicyKit (optional)

To build in directory build/:
    `meson build/ && ninja -C build`

Useful commands:
----------------
  * `ninja -v some/target`
  * `ninja test`
  * `sudo ninja install`
  * `DESTDIR=... ninja install`
  * `make DEBUG=YES`
    The Makefile is a full convenience wrapper, that allows to use meson/ninja in
    Makefile compatible IDEs like CLion.
    Note: For maximum control you should use meson/ninja directly instead.

A tarball can be created with:
  `git archive --format=tar --prefix=nts-timesyncd-241/ v241 | xz > nts-timesyncd-241.tar.xz`
