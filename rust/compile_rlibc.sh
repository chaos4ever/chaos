#!/bin/sh

set -e

#rm -rf rlibc-master
#wget https://github.com/alexcrichton/rlibc/archive/master.zip -O rlibc.zip
#unzip -q rlibc.zip && rm rlibc.zip
#rustc -C opt-level=2 --crate-type staticlib -g rlibc-master/src/lib.rs --out-dir .
rustc \
    -C opt-level=2 \
    -Z no-landing-pads \
    --crate-type rlib \
    -L .\
    --target=i686-unknown-elf.json \
    -v \
    -o librlibc.rlib \
    rlibc-master/src/lib.rs
