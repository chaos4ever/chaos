# Common settings and Rake rules for all libraries.

Rake.application.options.rakelib = "#{File.dirname(__FILE__)}/../rakelib" if Rake.application.options.rakelib.first == 'rakelib'

load "#{File.dirname(__FILE__)}/constants.rake"

task :default => [ :banner, OUTPUT ] do
  puts
end

task :banner do
  print 'Compiling '.bold
  print OUTPUT.sub('.a', '').cyan.bold
  puts '...'

  print '    '
end

file OUTPUT => OBJECTS do |t|
  puts
  puts '    Creating archive...'.blue.bold
  sh "#{AR} r --target=elf32-i386 #{t.name} #{t.prerequisites.join ' '}"
  cp t.name, '..'
end

# TODO: Check if we can get Rake's automatic 'clean' support working, so we don't have to do this manually.
task :clean do
  rm_f OBJECTS
end
