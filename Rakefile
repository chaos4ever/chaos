# Top-level Rakefile which is responsible for running all the other Rakefiles.

# TODO: Uncomment the rest here as soon as we have updated their build process to rake also.
folders = [:storm, :libraries, :servers]#, :programs]

verbose false

root = pwd()

# Need to set this up using a fully qualified path name, since the Rakefiles in the subfolders won't be able to find the custom
# .rake files otherwise.
Rake.application.options.rakelib = ["#{root}/rakelib"]

desc "Compiles chaos"
task :default => folders

desc "Performs cleanup (removes old .o files and similar)"
task :clean do
  folders.each do |folder|
    sh "cd #{folder} && #{RAKE_COMMAND} clean"
  end
end

desc "Compiles and installs chaos"
task :install do
  folders.each do |folder|
    sh "cd #{folder} && #{RAKE_COMMAND} install"
  end
end

desc "Compiles the 'storm' kernel."
task :storm do |folder|
  sh "cd #{folder} && #{RAKE_COMMAND}"
end

task :libraries => [ :storm ] do |folder|
  sh "cd #{folder} && #{RAKE_COMMAND}"
end

task :servers do |folder|
  sh "cd #{folder} && rake"
end

#task :programs do |folder|
#  system "cd #{folder} && rake"
#end
