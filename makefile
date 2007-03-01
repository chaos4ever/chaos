# $Id$

# Top-level makefile for compiling chaos. Order is incredibly
# important here. Kernel first, obviously, then libraries, then programs
# (not servers, because the programs are copied to a binary floppy image
# in the initial ramdisk server), then servers.

.PHONY: install clean hyperclean

install:
	make -C storm install
	make -C libraries install
	make -C programs install
	make -C servers install

clean:
	make -C storm clean
	make -C libraries clean
	make -C programs clean
	make -C servers clean

hyperclean:
	rm -rf $(PREFIX)/data
	rm -rf $(PREFIX)/programs
	rm -rf $(PREFIX)/system
