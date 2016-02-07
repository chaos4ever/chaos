#!/usr/bin/ruby
# frozen_string_literal: true
# rubocop:disable Semicolon

require 'English'

@return_value = 0

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
  line_count += 1
  $LAST_READ_LINE.gsub!(/ OK /) { |s| green s }
  $LAST_READ_LINE.gsub!(/  PASSED/) { |s| green s }
  $LAST_READ_LINE.gsub!(/  FAILED/) { |s| @return_value = 1; red s; }
  $LAST_READ_LINE.gsub!(/\d+ FAILED.*/) { |s| @return_value = 1; red s; }

  puts $LAST_READ_LINE
end

# No lines received = something went wrong with the test running.
exit 1 if line_count == 0

exit @return_value
