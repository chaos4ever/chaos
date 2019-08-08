#
# Dockerfile for building a Docker image with a chaos ISO image, as
# well as everything needed to run it in console mode (=qemu)
#

#
# Build image. Contains everything needed to build a fully functional
# chaos .iso image.
#
FROM debian AS build

RUN apt-get update && \
    apt-get install -y \
        dosfstools \
        gcc-7 \
        gcc-7-multilib \
        genisoimage \
        git \
        grub \
        grub-pc-bin \
        mtools \
        nasm \
        rake \
        ruby \
        xorriso
RUN echo 'drive u: file="/build/servers/block/initial_ramdisk/ramdisk.image"' > /etc/mtools.conf

COPY . /build/
WORKDIR /build

ENV ARCH=x86
ENV CC=gcc-7
RUN rake && rake install

#
# Runtime image. minideb is a smallish image, optimized for size.
# https://github.com/bitnami/minideb
#
FROM bitnami/minideb:stretch
COPY --from=build /build/chaos.iso /chaos/chaos.iso

RUN install_packages \
        qemu-system-x86

CMD qemu-system-i386 \
        -m 128 \
        -curses \
        -cdrom /chaos/chaos.iso
