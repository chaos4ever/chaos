## $Id$
## Author: Per Lundberg <plundis@chaosdev.org>

## Copyright 1999-2000 chaos development.
## Copyright 2007 chaos development.

#
# TODO: the makefiles in the subdirectories here are pretty much
# identical... we should try and move the generic content to a
# template that we include from all the makefiles.
#

include ../install.config

# List all the servers you want to compile here.

SUB_DIRECTORIES = network system file_system sound other video block

# Specifies the installation prefix (now taken from the global
# install.config file). Remember to do a make configure after changing
# the root!

PREFIX = $(CHAOS_ROOT)

.PHONY: all autochaos clean config install tags

all:
	for e in $(SUB_DIRECTORIES) ; do $(MAKE) -C $$e || exit ; done

clean:
	for e in $(SUB_DIRECTORIES) ; do $(MAKE) -C $$e clean || exit ; done

autochaos:
	for e in $(SUB_DIRECTORIES) ; do $(MAKE) -C $$e autochaos || exit ; done

configure:
#	for e in $(SUB_DIRECTORIES) ; do cd $$e && ./configure --install-prefix $(PREFIX) || exit ; done
	for e in $(SUB_DIRECTORIES) ; do (cd $$e && PREFIX=$(prefix) make configure && cd ..) || exit ; done

install:
	for e in $(SUB_DIRECTORIES) ; do $(MAKE) -C $$e install || exit clean ; done

tags:
	for e in $(SUB_DIRECTORIES) ; do cd $$e ; etags *.[ch] ; cd .. ; done
