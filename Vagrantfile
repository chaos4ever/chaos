Vagrant.configure(2) do |config|
  config.vm.box = 'chef/debian-7.8'
  config.vm.provision 'shell', inline: <<-SHELL
    set -e
    sudo apt-get update
    sudo apt-get install -y \
      astyle \
      cmake \
      dosfstools \
      gcc-multilib \
      genisoimage \
      git \
      grub-legacy \
      mtools \
      nasm \
      qemu \
      rake

    cd /vagrant
    ./install_cmocka.sh

    echo 'drive u: file="/vagrant/servers/block/initial_ramdisk/ramdisk.image"' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc
  SHELL
end
