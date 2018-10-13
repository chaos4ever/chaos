# frozen_string_literal: true

# Common settings and Rake rules for all servers.

Rake.application.options.rakelib = ["#{File.dirname(__FILE__)}/../rakelib"] if Rake.application.options.rakelib.first == 'rakelib'

LIBRARIES_DIR = "#{File.dirname(__FILE__)}/../libraries".freeze

COMMON_CFLAGS = %w[
  -Waggregate-return
  -Wall
  -Wcast-align
  -Werror
  -Wno-error=suggest-attribute=noreturn
  -Wextra
  -Winline
  -Wmissing-declarations
  -Wmissing-noreturn
  -Wno-pointer-sign
  -Wpointer-arith
  -Wredundant-decls
  -Wshadow
  -Wsign-compare
  -pipe
  -O3
  -fno-builtin
  -funsigned-char
  -g
  -m32
  -fomit-frame-pointer
  -ffreestanding
].freeze

# TODO: Consider changing the rules in common_rules to presume that these are actually arrays. That requires us to modify all
# Rakefiles though.
EXTRA_CFLAGS_POST ||= ''.freeze
CFLAGS = (COMMON_CFLAGS + %W[
  --std=gnu99
  -Wbad-function-cast
  -Wmissing-prototypes
  -Wnested-externs
  -Wstrict-prototypes
  #{EXTRA_CFLAGS_POST}
]).join(' ')

EXTRA_LDFLAGS_PRE ||= ''.freeze
LDFLAGS = %W[
  #{EXTRA_LDFLAGS_PRE}
  #{LIBRARIES_DIR}/startup.o
  -lgcc
  -static
  -nostdlib
  -Wl,-T,#{LIBRARIES_DIR}/chaos.ld
  -m32
  -L#{LIBRARIES_DIR}
].freeze

LIBRARY_FILES = LIBRARIES.map { |l| "#{LIBRARIES_DIR}/lib#{l}.a" }

programs_dir = File.dirname(__FILE__)

INCLUDES = %W[
  -I#{programs_dir}/../storm/include
  -I#{programs_dir}/../libraries
].freeze

task default: [:banner, OUTPUT] do
  puts
end

task :banner do
  print 'Compiling '.bold
  print OUTPUT.sub('.a', '').cyan.bold
  puts '...'

  print '    '
end

file OUTPUT => OBJECTS + LIBRARY_FILES do |t|
  begin
    puts
    puts "    Linking binary '#{OUTPUT}'...".blue.bold
    command = "#{CC} -o #{t.name} #{t.prerequisites.join ' '} #{LDFLAGS.join(' ')} -l#{LIBRARIES.join(' -l')}"
    sh command
  rescue StandardError
    puts "Error linking #{t.source}. Full command line was: #{command}"
    raise
  end
end

# TODO: Check if we can get Rake's automatic 'clean' support working, so we don't have to do this manually.
task :clean do
  rm_f OBJECTS
  rm_f OUTPUT
end

task install: OUTPUT do
  target_path = 'u:/programs'

  sh "mcopy -o #{OUTPUT} #{target_path}"

  puts "    Installed #{OUTPUT} in #{target_path}".gray
end
