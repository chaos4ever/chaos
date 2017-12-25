# frozen_string_literal: true

DEFINES = (ENV['DEFINES'] || '') + ' -DPACKAGE=\\"storm\\" -DVERSION=\\"git\\"'

# For a saner gdb experience, you typically want to compile without optimizations unless specifically making a release.
OPTIMIZATION_FLAG = ENV['RELEASE'] ? '-O3' : '-O0'

COMMON_CFLAGS =
  "-Wall -Wextra -Wshadow -Wpointer-arith -Waggregate-return -Wredundant-decls \
  -Winline -Werror -Wcast-align -Wsign-compare -Wmissing-declarations \
  -Wmissing-noreturn -pipe #{OPTIMIZATION_FLAG} -fno-builtin -funsigned-char \
  -g -m32 -fomit-frame-pointer -ffreestanding #{ENV['EXTRA_CFLAGS']} #{DEFINES} ".freeze

CFLAGS = COMMON_CFLAGS +
  "--std=gnu99 -Wbad-function-cast -Wmissing-prototypes -Wnested-externs \
  -Wstrict-prototypes"
CCFLAGS = COMMON_CFLAGS + '--std=gnu++11 ' #-Wno-c++0x-compat"
