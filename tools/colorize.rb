#!/usr/bin/ruby

$return_value = 0

def colorize(s, color_code)
  "\e[#{color_code}m#{s}\e[0m"
end

def red(s)
  colorize s, 31
end

def green(s)
  colorize s, 32
end

line_count = 0

while gets
  line_count = line_count + 1
  $_.gsub!(/ OK /) { |s| green s }
  $_.gsub!(/  PASSED/) { |s| green s }
  $_.gsub!(/  FAILED/) { |s| $return_value = 1; red s; }
  $_.gsub!(/\d+ FAILED.*/) { |s| $return_value = 1; red s; }

  puts $_
end

# No lines received = something went wrong with the test running.
exit 1 if line_count == 0

exit $return_value
