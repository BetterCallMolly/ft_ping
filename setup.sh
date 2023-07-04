#!/bin/bash

wget 'http://ftp.de.debian.org/debian/pool/main/i/inetutils/inetutils-ping_2.0-1+deb11u1_amd64.deb' -O inetutils-ping.deb

ar x inetutils-ping.deb
tar -xf data.tar.xz
rm data.tar.xz
rm control.tar.xz
rm debian-binary
rm inetutils-ping.deb

mv bin/ping ./ping
rm -rf bin
rm -rf usr