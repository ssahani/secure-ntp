#### Introduction
============

Configures NTP using Key Establishment (NTS-KE) protocol. `nts-timesyncd` is the systemd project's 
`timesyncd`, extracted out to be a standalone daemon.  It allows to Computers use the 
Network Time Protocol (NTP) to synchronize
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

#### Contributing
============

nts-timesyncd was branched from systemd version 253. It implements
Network Time Security for the Network Time Protocol.
RFC: https://datatracker.ietf.org/doc/html/rfc8915 using GnuTls
https://www.gnutls.org/

To contribute to nts-timesyncd, fork the current source code from github:
https://github.com/ssahani/secure-ntp
Send a pull request for the changes you like.

If you do not have a github account, the nts-timesyncd wiki page at
 https://github.com/ssahani/secure-ntp/wiki
lists further possibilities to contact the maintainers.

Finally, bug reports:
  https://github.com/ssahani/secure-ntp/issues

#### License
=======

LGPLv2.1+ for all code
  - except `src/basic/siphash24.c` which is CC0 Public Domain

#### Dependencies
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
```
  ❯ rm -rf build;meson setup build;
  ❯ make
  ❯ sudo make install
```

```
❯ systemctl daemon-reload
❯ systemctl start nts-timesyncd.service
```

```
❯ sudo systemctl status nts-timesyncd.service
● nts-timesyncd.service - Network Time Synchronization using ntpsec
     Loaded: loaded (/usr/lib/systemd/system/nts-timesyncd.service; disabled; preset: disabled)
    Drop-In: /usr/lib/systemd/system/service.d
             └─10-timeout-abort.conf
     Active: active (running) since Wed 2023-06-28 21:52:58 IST; 1min 0s ago
       Docs: man:nts-timesyncd.service.service(8)
   Main PID: 594513 (nts-timesyncd)
     Status: "Contacted time server [2a01:3f7:3:51::5]:123 (2a01:3f7:3:51::5)."
      Tasks: 2 (limit: 15145)
     Memory: 15.0M
        CPU: 122ms
     CGroup: /system.slice/nts-timesyncd.service
             └─594513 /usr/lib/systemd/nts-timesyncd

Jun 28 21:52:58 Zeus systemd[1]: Starting nts-timesyncd.service - Network Time Synchronization using ntpsec...
Jun 28 21:52:58 Zeus systemd[1]: Started nts-timesyncd.service - Network Time Synchronization using ntpsec.
Jun 28 21:53:00 Zeus nts-timesyncd[594513]: Contacted time server [2a01:3f7:3:51::5]:123 (2a01:3f7:3:51::5).
Jun 28 21:53:00 Zeus nts-timesyncd[594513]: Initial clock synchronization to Wed 2023-06-28 21:53:00.641044 IST.
```


#### Example configration
------------

```
❯ sudo useradd nts-timesync -G nts-timesync -M
❯ sudo chown nts-timesync:nts-timesync -R /etc/nts-timesync
❯ sudo cat /etc/systemd/nts-timesyncd.conf

```

```
[Time]
NetworkTimeSecurityServer=time.cloudflare.com nts.sth1.ntp.se nts.netnod.se
#NTP=
#FallbackNTP=time1.google.com time2.google.com time3.google.com time4.google.com
#RootDistanceMaxSec=5
#PollIntervalMinSec=32
#PollIntervalMaxSec=2048

```
