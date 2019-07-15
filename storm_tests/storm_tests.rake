# frozen_string_literal: true

Rake.application.options.rakelib = ["#{File.dirname(__FILE__)}/../rakelib"] if Rake.application.options.rakelib.first == 'rakelib'

# For a saner gdb experience, you typically want to compile without optimizations unless specifically making a release.
OPTIMIZATION_FLAG = ENV['RELEASE'] ? '-O3' : '-O0'

# FIXME: Don't repeat all of these in many places.
COMMON_CFLAGS = %W[
  -Wall
  -Wextra
  -Wshadow
  -Wpointer-arith
  -Waggregate-return
  -Wredundant-decls
  -Winline
  -Werror
  -Wcast-align
  -Wsign-compare
  -Wmissing-declarations
  -Wmissing-noreturn
  -pipe
  #{OPTIMIZATION_FLAG}
  -funsigned-char
  -g
  -m32
  -fomit-frame-pointer
].freeze

CFLAGS = COMMON_CFLAGS + %w[
  --std=gnu99
  -Wbad-function-cast
  -Wno-missing-declarations
  -Wno-unused-parameter
  -Wnested-externs
  -Wstrict-prototypes
]

INCLUDES = %w[
  -I../../storm/include
].freeze

def run_test(test)
  sh "./#{test} 2>&1 | RUBYOPT='' #{Rake.application.options.rakelib.first}/../tools/colorize.rb"
end
