# Top-level Rakefile which is responsible for running all the other Rakefiles.

# TODO: Uncomment the rest here as soon as we have merged their build process to rake also.
folders = [ :storm ] #, :libraries, :servers, :programs ]

verbose false

root = pwd()

desc "Compiles chaos"
task :default => folders

desc "Performs cleanup (removes old .o files and similar)"
task :clean do
  folders.each do |folder|
    sh "cd #{folder} && rake -s -R #{root}/rakelib clean"
  end
end

desc "Compiles and installs chaos"
task :install do
  folders.each do |folder|
    sh "cd #{folder} && rake -s -R #{root}/rakelib install"
  end
end

desc "Compiles the 'storm' kernel."
task :storm do |folder|
  sh "cd #{folder} && rake -s -R #{root}/rakelib"
end

# TODO: Not yet functional.
#task :libraries do |folder|
#  system "cd #{folder} && rake"
#end

#task :servers do |folder|
#  system "cd #{folder} && rake"
#end

#task :programs do |folder|
#  system "cd #{folder} && rake"
#end