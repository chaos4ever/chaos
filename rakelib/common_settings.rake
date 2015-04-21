verbose false

UNAME = `uname`.strip

case UNAME
when 'Darwin' then
  abort 'Error: OS X is not supported as a build host.'
else
  CC = ENV['CC'] || 'gcc-4.7'
  AR = ENV['AR'] || 'ar'
end

# Can always use plain nasm, since even the OSX version can produce ELF images.
NASM = 'nasm'

TARGET_ARCH = 'ia32'
RAKE_COMMAND = "rake -s -N -R #{Rake.application.options.rakelib.first}"
INSTALL_ROOT = 'a:'
INSTALL_COMMAND = 'mcopy -D o'
