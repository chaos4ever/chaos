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

    cd /tmp
    rm -rf cmocka*
    wget https://cmocka.org/files/1.0/cmocka-1.0.1.tar.xz
    tar xf cmocka-1.0.1.tar.xz 
    cd cmocka-1.0.1
    mkdir build
    cd build
    cmake -DCMAKE_C_FLAGS=-m32 ..
    make
    make install
    
    cd /vagrant && bzip2 -dc misc/grub.img.bz2 > floppy.img
    echo 'drive a: file="/vagrant/floppy.img" 1.44m mformat_only' > /etc/mtools.conf
    echo 'cd /vagrant' >> /home/vagrant/.bashrc
    echo '-U' > /home/vagrant/.astylerc
    echo '-H' >> /home/vagrant/.astylerc
    echo '-S' >> /home/vagrant/.astylerc
  SHELL
end
