# Top-level Rakefile which is responsible for running all the other Rakefiles.

# TODO: Uncomment the rest here as soon as we have updated their build process to rake also.
FOLDERS = [:storm, :libraries, :programs, :servers]

verbose false

root = pwd

# Need to set this up using a fully qualified path name, since the Rakefiles in the subfolders won't be able to find the custom
# .rake files otherwise.
Rake.application.options.rakelib = ["#{root}/rakelib"]

desc 'Compiles chaos'
task :default => [:create_ramdisk_image] + FOLDERS + [:iso_image]

desc 'Performs cleanup (removes old .o files and similar)'
task :clean do
  FOLDERS.each do |folder|
    sh "cd #{folder} && #{RAKE_COMMAND} clean"
  end
end

desc 'Creates the initial ramdisk image where the programs and their data is stored'
task :create_ramdisk_image do
  sh "cd servers/block/initial_ramdisk && #{RAKE_COMMAND} create_ramdisk_image"
end

desc 'Compiles and installs chaos'
task :install => [:install_folders, :iso_image]

task :install_folders do
  FileUtils.rm_rf INSTALL_ROOT

  FOLDERS.each do |folder|
    sh "cd #{folder} && #{RAKE_COMMAND} install"
  end
end

desc 'Builds a bootable ISO image with the kernel, servers and programs.'
task :iso_image do

  FileUtils.mkdir_p "#{INSTALL_ROOT}/boot/grub"
  FileUtils.cp 'menu.lst', "#{INSTALL_ROOT}/boot/grub"

  # I suspect this is actually an x86 binary, even though it resides in a folder that seems to indicate the opposite.
  FileUtils.cp '/usr/lib/grub/x86_64-pc/stage2_eltorito', "#{INSTALL_ROOT}/boot/grub"

  print 'Creating ISO image...'.cyan.bold
  sh "genisoimage \
      -R \
      -b boot/grub/stage2_eltorito \
      -no-emul-boot \
      -boot-load-size 4 \
      -boot-info-table \
      -input-charset ascii \
      -quiet \
      -o chaos.iso #{INSTALL_ROOT}"
  puts ' done.'
end

desc "Compiles the 'storm' kernel."
task :storm do |folder|
  sh "cd #{folder} && #{RAKE_COMMAND}"
end

desc "Compiles the unit tests for the 'storm' kernel."
task :build_storm_tests => :storm do
  sh "cd storm_tests && #{RAKE_COMMAND}"
end

desc 'Runs the unit tests'
task :tests => [:build_storm_tests] do
  sh "cd storm_tests && #{RAKE_COMMAND} tests"
end

task :libraries => [:storm] do |folder|
  sh "cd #{folder} && #{RAKE_COMMAND}"
end

task :programs do |folder|
  system "cd #{folder} && rake"
end

task :servers do |folder|
  sh "cd #{folder} && rake"
end

