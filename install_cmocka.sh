#!/bin/bash

set -e -o pipefail

cd /tmp
rm -rf cmocka*
wget https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz
tar xf cmocka-1.1.5.tar.xz
cd cmocka-1.1.5
mkdir build
cd build
cmake -DCMAKE_C_FLAGS=-m32 ..
make
sudo make install
sudo sh -c 'echo /usr/local/lib >> /etc/ld.so.conf'
sudo /sbin/ldconfig
