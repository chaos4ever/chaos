# frozen_string_literal: true
verbose false

UNAME = `uname`.strip

case UNAME
when 'Darwin' then
  abort 'Error: OS X is not supported as a build host.'
else
  CC = ENV['CC'] || 'gcc-5'
  AR = ENV['AR'] || 'ar'
  RUSTC = 'rustc'.freeze
  RUSTCFLAGS = '-O -C code-model=kernel -C relocation-model=static'.freeze
end

# Can always use plain nasm, since even the OSX version can produce ELF images.
NASM = 'nasm'.freeze

TARGET_ARCH = 'x86'.freeze
RAKE_COMMAND = "rake -s -N -R #{Rake.application.options.rakelib.first}".freeze
INSTALL_ROOT = '/tmp/chaos-iso-build'.freeze
INSTALL_COMMAND = 'install -D'.freeze
