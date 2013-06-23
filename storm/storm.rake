DEFINES = (ENV['DEFINES'] || '') + " -DPACKAGE=\\\"storm\\\" -DPACKAGE_VERSION=\\\"0.5.1+\\\" -DREVISION=\\\"`git rev-list HEAD --max-count 1 --abbrev-commit`\\\" -DCREATOR=\\\"`whoami`@`hostname -s`\\\""

COMMON_CFLAGS =
"-Wall -Wextra -Wshadow -Wpointer-arith -Waggregate-return -Wredundant-decls \
-Winline -Werror -Wcast-align -Wsign-compare -Wmissing-declarations \
-Wmissing-noreturn -pipe -O3 -fno-builtin -funsigned-char \
-g -m32 -fomit-frame-pointer -ffreestanding #{ENV['EXTRA_CFLAGS']} #{DEFINES} "

CFLAGS = COMMON_CFLAGS + 
"--std=gnu99 -Wbad-function-cast -Wmissing-prototypes -Wnested-externs \
-Wstrict-prototypes"
CCFLAGS = COMMON_CFLAGS + "--std=gnu++11 " #-Wno-c++0x-compat"

INCLUDES = %w(
  -I../include
  -I..
  -I.
)

file OUTPUT => OBJECTS do |t|
  puts
  puts '    Creating archive...'.blue.bold
  sh "#{AR} r --target=elf32-i386 #{t.name} #{t.prerequisites.join ' '}"
end

# TODO: Check if we can get Rake's automatic 'clean' support working, so we don't have to do this manually.
task :clean do
  rm_f OBJECTS
  rm_f OUTPUT
end
