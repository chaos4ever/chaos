## $Id$
## Process this file with automake to produce Makefile.in

## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of the
## License, or (at your option) any later version.

## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
## USA

# HTML is not supported by Automake yet, so we have to do some stuff
# ourselves for now..

targets = server-architecture.html \
the_chaos_programming_reference_manual.html coding-standards.html \
filesystem-hierarchy.html a_new_file_system.html

all-local: $(targets)

# SGML files generate multiple output files, so we just clean out
# every HTML file in this directory.

clean-local:
	rm -f *.html *~

%.html: %.texi
	makeinfo --html $<

%.html: %.sgml
	sgmltools $<