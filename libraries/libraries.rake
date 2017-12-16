# frozen_string_literal: true

# Common settings and Rake rules for all libraries.

Rake.application.options.rakelib = ["#{File.dirname(__FILE__)}/../rakelib"] if Rake.application.options.rakelib.first == 'rakelib'

load "#{File.dirname(__FILE__)}/constants.rake"

task default: [:banner, OUTPUT] do
  puts
end

task :banner do
  print 'Compiling '.bold
  library = (/lib(.+)\.a/.match OUTPUT)
  print library[1].cyan.bold
  puts ' library...'.bold

  print '    '
end

file OUTPUT => OBJECTS do |t|
  puts
  puts '    Creating archive...'.blue.bold

  # Redirecting stderr is incredibly ugly, but the problem is that I haven't found any other way to silence the default output
  # from ar...
  sh "#{AR} r --target=elf32-i386 #{t.name} #{t.prerequisites.join ' '} 2> /dev/null"

  cp t.name, '..'
end

# TODO: Check if we can get Rake's automatic 'clean' support working, so we don't have to do this manually.
task :clean do
  rm_f OBJECTS
  rm_f OUTPUT
end

# The libraries aren't really "installed", but we need to support this target anyway so that you can run a top-level
# "rake install" to get it all installed.
task :install
