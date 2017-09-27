# frozen_string_literal: true
# rubocop:disable GuardClause

verbose false

UNAME = `uname`.strip

case UNAME
when 'Darwin' then
  abort 'Error: OS X is not supported as a build host.'
else
  SUPPORTED_ARCHITECTURES = %w(x86 raspberrypi).freeze

  unless ENV['ARCH']
    raise 'You must set the ARCH environment variable to one of the supported architectures ' \
          "(#{SUPPORTED_ARCHITECTURES.join(', ')})"
  end

  unless SUPPORTED_ARCHITECTURES.include?(ENV['ARCH'])
    raise "The ARCH environment variable contains an invalid target architecture (#{ENV['ARCH']}). It must be set " \
          "to one of the supported architectures (#{SUPPORTED_ARCHITECTURES.join(', ')})"
  end

  TARGET_ARCH ||= ENV['ARCH']

  if TARGET_ARCH == 'x86'
    CC = ENV['CC'] || 'gcc-5'
    AR = ENV['AR'] || 'ar'
    AR_TARGET = '--target=elf32-i386'.freeze
    NASM = 'nasm'.freeze

    extra_cflags = '-m32'
    extra_cflags += ' -Werror=implicit-fallthrough=3' if CC =~ /gcc-7/
    ARCH_CFLAGS = extra_cflags.freeze
  elsif TARGET_ARCH == 'raspberrypi'
    CC = ENV['CC'] || 'arm-none-eabi-gcc'
    AR = ENV['AR'] || 'arm-none-eabi-ar'
    AR_TARGET = ''.freeze
    ARCH_CFLAGS = '-mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s'.freeze
  else
    raise "Unsupported target architecture #{TARGET_ARCH}"
  end

  RUSTC = 'rustc'.freeze
  RUSTCFLAGS = '-O -C code-model=kernel -C relocation-model=static'.freeze
end

RAKE_COMMAND = "rake -s -N -R #{Rake.application.options.rakelib.first}".freeze
INSTALL_ROOT = '/tmp/chaos-iso-build'.freeze
INSTALL_COMMAND = 'install -D'.freeze
