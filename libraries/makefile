## $Id$
## Authors: Henrik Hallin <hal@chaosdev.org>
##          Per Lundberg <plundis@chaosdev.org>

## Copyright 1999-2000 chaos development.
## Copyright 2007 chaos development.

include ../install.config

LIBRARIES = string system ipc execute_elf memory unicode ipv4 \
video serial console random log file math checksum mutex pci time \
list sound

# Specifies the installation prefix (now taken from the global
# install.config file). Remember to do a make configure after changing
# the root!

PREFIX = $(CHAOS_ROOT)

CFLAGS = -Wall -W -Wshadow -Wpointer-arith -Waggregate-return \
-Wstrict-prototypes -Wredundant-decls -Winline -Wmissing-prototypes \
-Werror -Wcast-align -Wbad-function-cast -Wsign-compare \
-Wmissing-declarations -pipe -Wnested-externs -O3 -fno-builtin \
-funsigned-char -g -m32

INCLUDES = -I. -I$(PREFIX)/data/programming/c/headers

CC = gcc

%.o: %.c
	$(CC) -o $(@) $(CFLAGS) $(INCLUDES) $(DEFS) -c $<

.PHONY: all autochaos clean config install

all:
	for e in $(LIBRARIES) ; do $(MAKE) -C $$e || exit ; done
	$(MAKE) startup.o

clean:
	for e in $(LIBRARIES) ; do $(MAKE) -C $$e clean || exit ; done
	rm -f startup.o

autochaos:
	for e in $(LIBRARIES) ; do cd $$e && pwd && autochaos || exit ; cd .. ; done

configure:
	for e in $(LIBRARIES) ; do cd $$e && ./configure --install-prefix $(PREFIX) || exit ; cd .. ; done

install:
	for e in $(LIBRARIES) ; do $(MAKE) -C $$e install || exit ; done
	mkdir -p $(PREFIX)/data/programming/linker
	cp chaos.ld $(PREFIX)/data/programming/linker
	mkdir -p $(PREFIX)/data/programming/c/startup
	$(MAKE) startup.o
	cp startup.o $(PREFIX)/data/programming/c/startup
