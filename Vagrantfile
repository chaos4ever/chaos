# frozen_string_literal: true

Vagrant.configure("2") do |config|
  config.vm.box = "roboxes-x32/debian11"

  # Sometimes, the default seems to be too little for this one.
  config.vm.boot_timeout = 600
  # When using libvirt it's usefull to explicitly tell Vagrant what method
  # of folder syncing to use.
  config.vm.synced_folder "./", "/vagrant", type: "rsync"
  config.vm.provision 'shell', inline: <<-SHELL
    set -e

    # Override sources.list for faster downloads (for people outside of the US).
    echo deb http://httpredir.debian.org/debian/ bullseye main > /etc/apt/sources.list
    echo deb-src http://httpredir.debian.org/debian/ bullseye main >> /etc/apt/sources.list
    # echo deb http://security.debian.org/debian-security bullseye/updates main >> /etc/apt/sources.list
    # echo deb-src http://security.debian.org/debian-security bullseye/updates main >> /etc/apt/sources.list

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
      psmisc \
      qemu \
      gcc-arm-none-eabi \
      xorriso \
      rake

    # The gcc-arm-none-eabi package is not yet available in stretch:
    # https://packages.debian.org/jessie/devel/gcc-arm-none-eabi
    #echo deb http://httpredir.debian.org/debian/ unstable main >> /etc/apt/sources.list
    #sudo apt-get update
    #sudo DEBIAN_FRONTEND=noninteractive apt-get install -y gcc-arm-none-eabi

    # Disabled for now since we don't have any Rust dependencies, and it slows things down when setting the VM up.
    ## We need the beta channel for the #![feature] functionality.
    #curl -sSf https://static.rust-lang.org/rustup.sh > /tmp/rustup.sh && \
    #  sh /tmp/rustup.sh --yes --channel=nightly --date=2016-01-13

    cd /vagrant
    ./install_cmocka.sh

    echo 'drive u: file="/vagrant/servers/block/initial_ramdisk/ramdisk.image"' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo 'export ARCH=x86' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc

    echo target remote localhost:1234 > /home/vagrant/.gdbinit
  SHELL
end
