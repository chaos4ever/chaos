verbose false

# FIXME: Should consider supporting a toplevel Rakefile.local or something where you can define this, so you don't have to
# modify a version-controlled file.
CC = ENV['CC'] || 'x86_64-pc-linux-gcc'
AR = ENV['AR'] || 'x86_64-pc-linux-ar'

TARGET_ARCH = 'ia32'
RAKE_COMMAND = "rake -s -R #{Rake.application.options.rakelib}"
INSTALL_ROOT = '/Volumes/chaos'
