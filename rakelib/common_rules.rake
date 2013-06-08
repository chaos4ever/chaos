rule '.o' => [ '.c' ] do |t|
  begin
    print((t.source + ' ').cyan)
    command = "#{CC} -o #{t.name} #{CFLAGS} #{INCLUDES.join(' ')} -c #{t.source}"
    sh command
  rescue
    puts "Error compiling #{t.source}. Full command line was: #{command}"
    raise
  end
end

rule '.o' => [ '.cc' ] do |t|
  begin
    print((t.source + ' ').cyan)
    command = "#{CC} -o #{t.name} #{CCFLAGS} #{INCLUDES.join(' ')} -c #{t.source}"
    sh command
  rescue
    puts "Error compiling #{t.source}. Full command line was: #{command}"
    raise
  end
end

rule '.o' => [ '.S' ] do |t|
  begin
    print((t.source + ' ').cyan)
    command = "#{CC} -o #{t.name} #{CFLAGS} #{INCLUDES.join(' ')} -c #{t.source}"
    sh command
  rescue
    puts "Error compiling #{t.source}. Full command line was: #{command}"
    raise
  end
end
