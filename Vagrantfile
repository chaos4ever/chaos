# frozen_string_literal: true
Vagrant.configure(2) do |config|
  config.vm.box = 'fujimakishouten/debian-buster64'

  config.vm.provision 'shell', inline: <<-SHELL
    set -e

    # Override sources.list for faster downloads (for people outside of the US).
    echo deb http://httpredir.debian.org/debian/ buster main > /etc/apt/sources.list
    echo deb-src http://httpredir.debian.org/debian/ buster main >> /etc/apt/sources.list
    echo deb http://security.debian.org/debian-security buster/updates main >> /etc/apt/sources.list
    echo deb-src http://security.debian.org/debian-security buster/updates main >> /etc/apt/sources.list

    echo "127.0.0.1 debian-buster" | sudo tee /etc/hosts

    sudo apt-get update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
      astyle \
      cmake \
      dosfstools \
      gcc-6-multilib \
      gcc-arm-none-eabi \
      gdb \
      genisoimage \
      git \
      grub-legacy \
      mtools \
      nasm \
      qemu \
      rake

    # Disabled for now since we don't have any Rust dependencies, and it slows things down when setting the VM up.
    ## We need the beta channel for the #![feature] functionality.
    #curl -sSf https://static.rust-lang.org/rustup.sh > /tmp/rustup.sh && \
    #  sh /tmp/rustup.sh --yes --channel=nightly --date=2016-01-13

    # Disabled for now since it doesn't compile on buster for unknown reasons.
    #cd /vagrant
    #./install_cmocka.sh

    echo 'drive u: file="/vagrant/servers/block/initial_ramdisk/ramdisk.image"' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo 'export ARCH=x86' >> /home/vagrant/.bashrc
    echo 'export CC=gcc-6' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc

    echo target remote localhost:1234 > /home/vagrant/.gdbinit
  SHELL
end
