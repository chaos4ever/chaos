#!/bin/sh

set -e

RUST_CHANGESET=$(rustc -v --version | grep commit-hash | awk '{ print $2 }')

if [ ! -d rust-$RUST_CHANGESET ]; then
  rm -rf rust
  wget https://github.com/rust-lang/rust/archive/$RUST_CHANGESET.zip -O rust.zip
  unzip -q rust.zip && rm rust.zip
fi

rustc \
    -C opt-level=2 \
    -Z no-landing-pads \
    --crate-type rlib \
    -g \
    --target i686-unknown-elf.json \
    -v \
    rust-$RUST_CHANGESET/src/libcore/lib.rs \
    --out-dir .
