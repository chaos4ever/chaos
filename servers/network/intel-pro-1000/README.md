# README

Intel Pro/1000 driver, based on the MINIX 3 driver as found here:
https://github.com/Stichting-MINIX-Research-Foundation/minix/tree/master/minix/drivers/net/e1000
(commit d4dd651)

That driver is in turn partially based on the DragonflyBSD (FreeBSD) implementation, and the fxp driver for Minix 3. Thanks to both of these projects for gratuitously sharing your hard efforts using very liberal software licenses.

The following files are more or less used from that driver right away, with very few local changes:

* e1000_reg.h
* e1000_pci.h
