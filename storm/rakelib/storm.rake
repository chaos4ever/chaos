# frozen_string_literal: true

DEFINES = (ENV['DEFINES'] || '') +
  ' -DPACKAGE_NAME=\\"storm\\" -DPACKAGE_VERSION=\\"0.5.1+\\" -DREVISION=\\"`git rev-list HEAD --max-count 1 --abbrev-commit`\\" \
  -DCREATOR=\\"`whoami`@`hostname -s`\\"'

# For a saner gdb experience, you typically want to compile without optimizations unless specifically making a release.
OPTIMIZATION_FLAG = ENV['RELEASE'] ? '-O3' : '-O0'

COMMON_CFLAGS =
  "-Wall -Wextra -Wshadow -Wpointer-arith -Waggregate-return -Wredundant-decls \
  -Winline -Werror -Wno-error=address-of-packed-member -Wcast-align -Wsign-compare -Wmissing-declarations \
  -Wmissing-noreturn -pipe #{OPTIMIZATION_FLAG} -fno-builtin -fno-asynchronous-unwind-tables -funsigned-char \
  -g -fomit-frame-pointer -ffreestanding #{ENV['EXTRA_CFLAGS']} #{DEFINES} ".freeze

CFLAGS = COMMON_CFLAGS +
  "--std=gnu99 -Wbad-function-cast -Wmissing-prototypes -Wnested-externs \
  -Wstrict-prototypes " + ARCH_CFLAGS

INCLUDES = %w[
  -I../include
  -I..
  -I.
].freeze

file OUTPUT => objects do |t|
  puts
  puts '    Creating archive...'.blue.bold
  sh "#{AR} r #{AR_TARGET} #{t.name} #{t.prerequisites.join ' '}"
end

# TODO: Check if we can get Rake's automatic 'clean' support working, so we don't have to do this manually.
task :clean do
  rm_f objects
  rm_f OUTPUT
end
