# Common settings and Rake rules for all libraries.

Rake.application.options.rakelib = "#{File.dirname(__FILE__)}/../rakelib" if Rake.application.options.rakelib.first == 'rakelib'

LIBRARIES_DIR = "#{File.dirname(__FILE__)}/../libraries"

COMMON_CFLAGS = %w(
  -Wall -Wextra -Wshadow -Wpointer-arith -Waggregate-return -Wredundant-decls -Winline -Werror -Wcast-align -Wsign-compare 
  -Wmissing-declarations -Wmissing-noreturn -pipe -O3 -fno-builtin -funsigned-char -g -m32 -fomit-frame-pointer -ffreestanding
 )

# TODO: Consider changing the rules in common_rules to presume that these are actually arrays. That requires us to modify all Rakefiles though.
CFLAGS = (COMMON_CFLAGS + %w(
  --std=gnu99 -Wbad-function-cast -Wmissing-prototypes -Wnested-externs -Wstrict-prototypes
)).join(' ')
CCFLAGS = (COMMON_CFLAGS + %w(
  --std=gnu++11 -Wno-inline
)).join(' ')

LDFLAGS = %W(
  #{LIBRARIES_DIR}/startup.o
  -nostdlib
  -Wl,-T,#{LIBRARIES_DIR}/chaos.ld
  -m32
  -L#{LIBRARIES_DIR}
)

servers_dir = File.dirname(__FILE__)

INCLUDES = %W(
  -I#{servers_dir}/../storm/include
  -I#{servers_dir}/../libraries
)

task :default => [ :banner, OUTPUT ] do
  puts
end

task :banner do
  print "Compiling ".bold
  print OUTPUT.sub('.a', '').cyan.bold
  puts "..."

  print "    "
end

file OUTPUT => OBJECTS do |t|
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
end
