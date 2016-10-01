# frozen_string_literal: true
Vagrant.configure(2) do |config|
  config.vm.box = 'remram/debian-9-i386'

  config.vm.provision 'shell', inline: <<-SHELL
    set -e

    # Override sources.list for faster downloads (for people outside of the US).
    echo deb http://httpredir.debian.org/debian/ stretch main > /etc/apt/sources.list
    echo deb-src http://httpredir.debian.org/debian/ stretch main >> /etc/apt/sources.list
    echo deb http://security.debian.org/debian-security stretch/updates main >> /etc/apt/sources.list
    echo deb-src http://security.debian.org/debian-security stretch/updates main >> /etc/apt/sources.list

    sudo apt-get update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
      astyle \
      cmake \
      dosfstools \
      gcc-multilib \
      gdb \
      genisoimage \
      git \
      grub-legacy \
      mtools \
      nasm \
      qemu \
      rake

    # The gcc-arm-none-eabi package is not yet available in stretch:
    # https://packages.debian.org/jessie/devel/gcc-arm-none-eabi
    echo deb http://httpredir.debian.org/debian/ unstable main >> /etc/apt/sources.list
    sudo apt-get update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y gcc-arm-none-eabi

    # Disabled for now since we don't have any Rust dependencies, and
    ## We need the beta channel for the #![feature] functionality.
    #curl -sSf https://static.rust-lang.org/rustup.sh > /tmp/rustup.sh && \
    #  sh /tmp/rustup.sh --yes --channel=nightly --date=2016-01-13

    cd /vagrant
    ./install_cmocka.sh

    echo 'drive u: file="/vagrant/servers/block/initial_ramdisk/ramdisk.image"' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc

    echo target remote localhost:1234 > /home/vagrant/.gdbinit
  SHELL
end
