# frozen_string_literal: true
# Common settings and Rake rules for all servers.

Rake.application.options.rakelib = ["#{File.dirname(__FILE__)}/../rakelib"] if Rake.application.options.rakelib.first == 'rakelib'

LIBRARIES_DIR = "#{File.dirname(__FILE__)}/../libraries".freeze

COMMON_CFLAGS = %w(
  -Wall
  -Wextra
  -Wshadow
  -Wpointer-arith
  -Waggregate-return
  -Wredundant-decls
  -Winline
  -Werror
  -Wcast-align
  -Wno-pointer-sign
  -Wsign-compare
  -Wmissing-declarations
  -Wmissing-noreturn
  -pipe
  -O0
  -fno-builtin
  -funsigned-char
  -ggdb3
  -m32
  -fomit-frame-pointer
  -ffreestanding
).freeze

# TODO: Consider changing the rules in common_rules to presume that these are actually arrays. That requires us to modify all
# Rakefiles though.
CFLAGS = (COMMON_CFLAGS + %w(
  --std=gnu99
  -Wbad-function-cast
  -Wmissing-prototypes
  -Wnested-externs
  -Wstrict-prototypes
)).join(' ')

EXTRA_LDFLAGS_PRE ||= ''.freeze
EXTRA_LIBS ||= ''.freeze
LDFLAGS = %W(
  #{EXTRA_LDFLAGS_PRE}
  #{LIBRARIES_DIR}/startup.o
  -nostdlib
  -Wl,-T,#{LIBRARIES_DIR}/chaos.ld
  -m32
  -L#{LIBRARIES_DIR}
  #{EXTRA_LIBS}
).freeze

LIBRARY_FILES = LIBRARIES.map { |l| "#{LIBRARIES_DIR}/lib#{l}.a" }

servers_dir = File.dirname(__FILE__)

INCLUDES = %W(
  -I#{servers_dir}/../storm/include
  -I#{servers_dir}/../libraries
).freeze

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
  rescue
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
  target_path = INSTALL_ROOT + '/servers'

  sh "#{INSTALL_COMMAND} #{OUTPUT} #{target_path}/#{OUTPUT}"
  sh "gzip -9f #{target_path}/#{OUTPUT}"
  puts "    Installed #{OUTPUT} in #{target_path}".gray
end
