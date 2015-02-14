Vagrant.configure(2) do |config|
  config.vm.box = "chef/debian-7.8"
  config.vm.provision 'shell', inline: <<-SHELL
     sudo apt-get update
     sudo apt-get install -y g++ git mtools nasm rake
     echo 'cd /vagrant' >> /home/vagrant/.bashrc
     echo 'drive a: file="/vagrant/floppy.img" 1.44m mformat_only' > /etc/mtools.conf
  SHELL
end
