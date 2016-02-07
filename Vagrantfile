# frozen_string_literal: true
Vagrant.configure(2) do |config|
  config.vm.box = 'remram/debian-9-i386'

  config.vm.provision 'shell', inline: <<-SHELL
    set -e
    sudo apt-get update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
      astyle \
      cmake \
      dosfstools \
      gcc-5 \
      gcc-multilib \
      gdb \
      genisoimage \
      git \
      grub-legacy \
      mtools \
      nasm \
      qemu \
      rake

    # We need the beta channel for the #![feature] functionality.
    curl -sSf https://static.rust-lang.org/rustup.sh > /tmp/rustup.sh && \
      sh /tmp/rustup.sh --yes --channel=nightly --date=2016-01-13

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
