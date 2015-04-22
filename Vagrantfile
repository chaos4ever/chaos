Vagrant.configure(2) do |config|
  config.vm.box = 'chef/debian-7.8'
  config.vm.provision 'shell', inline: <<-SHELL
    set -e
    sudo apt-get update
    sudo apt-get install -y \
      astyle \
      cmake \
      gcc-multilib \
      git \
      mtools \
      nasm \
      qemu \
      rake 

    cd /vagrant
    ./install_cmocka.sh
    echo /usr/local/lib >> /etc/ld.so.conf
    
    cd /vagrant && bzip2 -dc misc/grub.img.bz2 > floppy.img
    echo 'drive a: file="/vagrant/floppy.img" 1.44m mformat_only' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc
  SHELL
end
