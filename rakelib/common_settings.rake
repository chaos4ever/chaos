verbose false

# FIXME: Should consider supporting a toplevel Rakefile.local or something where you can define this, so you don't have to
# modify a version-controlled file.

UNAME = `uname`.strip

case UNAME
when 'Darwin' then
  abort 'Error: OS X is not supported as a build host.'
else
  CC = ENV['CC'] || 'gcc'
  AR = ENV['AR'] || 'ar'
end

# Can always use plain nasm, since even the OSX version can produce ELF images.
NASM = 'nasm'

TARGET_ARCH = 'ia32'
RAKE_COMMAND = "rake -s -N -R #{Rake.application.options.rakelib.first}"
INSTALL_ROOT = 'a:'
INSTALL_COMMAND = 'mcopy -D o'
