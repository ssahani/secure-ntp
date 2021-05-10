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

Finally, bug reports:
  https://github.com/nts-timesyncd/nts-timesyncd/issues

License
=======

LGPLv2.1+ for all code
  - except `src/basic/siphash24.c` which is CC0 Public Domain

Dependencies
============

  * glibc
  * libcap
  * libacl
  * libselinux (optional)
  * libaudit (optional)
  * GnuTLS
  * pkg-config
  * gperf
  * python, meson, ninja
  * gcc, awk, sed, grep, m4, and similar tools

To build in directory build/:
    - `make`
    or
    - `meson build/ && ninja -C build`

Useful commands:
----------------
  * `ninja -v some/target`
  * `ninja test`
  * `sudo ninja install`
  * `DESTDIR=... ninja install`
  * `make DEBUG=YES`

Example configration
------------

```
❯ sudo cat /etc/nts-timesync/nts-timesyncd.conf

[Time]
NetworkTimeSecurityServer=time.cloudflare.com nts.sth1.ntp.se nts.netnod.se
#NTP=
#FallbackNTP=time1.google.com time2.google.com time3.google.com time4.google.com
#RootDistanceMaxSec=5
#PollIntervalMinSec=32
#PollIntervalMaxSec=2048
ConnectionRetrySec=5

```
