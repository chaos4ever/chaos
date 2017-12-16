# frozen_string_literal: true

DEFINES = (ENV['DEFINES'] || '') + ' -DPACKAGE=\\"storm\\" -DVERSION=\\"git\\"'

COMMON_CFLAGS =
  "-Wall -Wextra -Wshadow -Wpointer-arith -Waggregate-return -Wredundant-decls \
  -Winline -Werror -Wcast-align -Wsign-compare -Wmissing-declarations \
  -Wmissing-noreturn -pipe -O3 -fno-builtin -funsigned-char \
  -g -m32 -fomit-frame-pointer -ffreestanding #{ENV['EXTRA_CFLAGS']} #{DEFINES} ".freeze

CFLAGS = COMMON_CFLAGS +
  "--std=gnu99 -Wbad-function-cast -Wmissing-prototypes -Wnested-externs \
  -Wstrict-prototypes"
CCFLAGS = COMMON_CFLAGS + '--std=gnu++11 ' #-Wno-c++0x-compat"
