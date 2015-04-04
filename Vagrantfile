Vagrant.configure(2) do |config|
  config.vm.box = "chef/debian-7.8"
  config.vm.provision 'shell', inline: <<-SHELL
     set -e
     sudo apt-get update
     sudo apt-get install -y g++ git mtools nasm rake astyle
     echo 'cd /vagrant' >> /home/vagrant/.bashrc
     cd /vagrant && bzip2 -dc misc/grub.img.bz2 > floppy.img
     echo 'drive a: file="/vagrant/floppy.img" 1.44m mformat_only' > /etc/mtools.conf
     echo '-U' > /home/vagrant/.astylerc
     echo '-H' >> /home/vagrant/.astylerc
     echo '-S' >> /home/vagrant/.astylerc
  SHELL
end
