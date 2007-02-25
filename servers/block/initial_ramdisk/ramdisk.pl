#!/usr/bin/perl -w

# $Id$
# Abstract: Get the block size of the given file and print in a C-like way.
# Author: Per Lundberg <plundis@chaosdev.org>

# Copyright 1999-2000 chaos development.

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA.

use strict;
use POSIX;

my $file = $ARGV[0] or exit;

(undef, undef, undef, undef, undef, undef, undef, my $size) = stat ($file);
print "#define BLOCK_SIZE       512\n";
print "#define NUMBER_OF_BLOCKS ", POSIX::ceil ($size / 512), "\n";
