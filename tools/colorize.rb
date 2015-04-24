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

while gets
  $_.gsub!(/ OK /) { |s| green s }
  $_.gsub!(/  PASSED/) { |s| green s }
  $_.gsub!(/  FAILED/) { |s| $return_value = 1; red s; }
  $_.gsub!(/\d+ FAILED.*/) { |s| $return_value = 1; red s; }

  puts $_
end

exit $return_value