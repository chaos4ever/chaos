# Top-level Rakefile which is responsible for importing all the other Rakefiles.

# TODO: Uncomment the rest here.
folders = [ :storm ] #, :libraries, :servers, :programs ]

verbose false

root = pwd()

task :default => folders

task :clean do
  folders.each do |folder|
    sh "cd #{folder} && rake clean"
  end
end

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