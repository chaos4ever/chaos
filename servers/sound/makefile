## $Id$
## Author: Per Lundberg <per@chaosdev.io>

## Copyright 1999-2000 chaos development.
## Copyright 2007 chaos development.

include ../../install.config

SERVERS = soundblaster
PREFIX = $(CHAOS_ROOT)

.PHONY: all autochaos clean config install tags

all:
	for e in $(SERVERS) ; do $(MAKE) -C $$e || exit ; done

clean:
	for e in $(SERVERS) ; do $(MAKE) -C $$e clean || exit ; done

autochaos:
	for e in $(SERVERS) ; do cd $$e && pwd && autochaos || exit ; cd .. ; done

configure:
	for e in $(SERVERS) ; do cd $$e && ./configure --install-prefix $(PREFIX) || exit ; cd .. ; done

install:
	for e in $(SERVERS) ; do $(MAKE) -C $$e install || exit clean ; done

tags:
	for e in $(SERVERS) ; do cd $$e ; etags *.[ch] ; cd .. ; done
