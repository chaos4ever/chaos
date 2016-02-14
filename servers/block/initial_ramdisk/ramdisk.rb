#!/usr/bin/env ruby
# frozen_string_literal: true
#
# Abstract: Get the block size of the given file and print it as a C header.
# Author: Per Lundberg <per@chaosdev.io>
#
# © Copyright 1999-2000 chaos development
# © Copyright 2015 chaos development

file = ARGV[0] or raise 'You need to provide the name of the file to inspect'

BLOCK_SIZE = 512

number_of_blocks = File.size(file).fdiv(BLOCK_SIZE).ceil

puts "#define BLOCK_SIZE       #{BLOCK_SIZE}\n"
puts "#define NUMBER_OF_BLOCKS #{number_of_blocks}\n"
