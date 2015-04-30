Rake.application.options.rakelib = ["#{File.dirname(__FILE__)}/../rakelib"] if Rake.application.options.rakelib.first == 'rakelib'

# FIXME: Don't repeat all of these in many places.
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
  -Wsign-compare
  -Wmissing-declarations
  -Wmissing-noreturn
  -pipe
  -O3
  -funsigned-char
  -m32
  -fomit-frame-pointer
)

CFLAGS = COMMON_CFLAGS + %w(
  --std=gnu99
  -Wbad-function-cast
  -Wno-missing-declarations
  -Wno-unused-parameter
  -Wnested-externs
  -Wstrict-prototypes
)

INCLUDES = %W(
  -I..
  -I../../storm/include
)

def run_test(test)
  sh "./#{test} 2>&1 | #{Rake.application.options.rakelib.first}/../tools/colorize.rb"
end