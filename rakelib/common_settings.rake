verbose false

UNAME = `uname`.strip

case UNAME
when 'Darwin' then
  abort 'Error: OS X is not supported as a build host.'
else
  CC = ENV['CC'] || 'gcc-5'
  AR = ENV['AR'] || 'ar'
  RUSTC = 'rustc'
  RUSTCFLAGS = %w(
    -O
    -C code-model=kernel
    -C relocation-model=static
    -Z no-landing-pads
    -g).join(' ')
end

# Can always use plain nasm, since even the OSX version can produce ELF images.
NASM = 'nasm'

TARGET_ARCH = 'ia32'
RAKE_COMMAND = "rake -s -N -R #{Rake.application.options.rakelib.first}"
INSTALL_ROOT = '/tmp/chaos-iso-build'
INSTALL_COMMAND = 'install -D'
