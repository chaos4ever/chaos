# README

![](https://raw.github.com/chaos4ever/chaos/master/misc/chaos-logo.png)

Welcome to chaos. chaos is an operating system developed around the year 2000 for the x86 (also referred to as "ia32" in the codebase) platform. It contains working protected mode/virtual memory initialization, processes and threads, interrupt support, PCI, text-based VGA and a simple IPv4 stack (supporting UDP, ARP and ICMP). Naturally, given the hobbyist nature of the project, none of the features we just mentioned is "complete" in the sense that it supports all features of the PCI bus, or all features of UDP, ICMP etc. Nonetheless, the system was at an earlier point stable to the point where it possible to e.g. use it for network bridging - you could run e.g. ssh "through" a machine running chaos.

Having that said, there are a number of issues with the codebase which made us start the development of the new [stormG3](https://github.com/chaos4ever/stormG3) kernel (which would replace the existing storm "G1" kernel. There was also a [stormG2](https://github.com/chaos4ever/stormG2) project being undertaken but it was abandoned before being completed). The most critical issue with stormG1 is that the kernel leaks memory; the number of free pages decrease gradually over time. Because of this and the general feeling that the system would benefit from a major refactoring (hey, isn't it always fun to rewrite your code? ;) the storm G3 project was initiated, where we attempted to Do Things Right&trade; to a greater point than had been earlier done. Like many hobby/spare-time projects, the project was never finished and storm G3 is therefore currently in a very unfinished state. You can still find the code for it at our old [Sourceforge site](https://sourceforge.net/projects/chaos/); we may possibly put the code up here on github also sometime in the future if there are people willing to try to fill in the gaps and finish the work to make that kernel be generally usable.

## Compiling chaos

### Prerequisites:

- A recent version of GCC (4.7 or newer recommended). If you are using Mac OS X, try [this link](http://crossgcc.rts-software.org/doku.php?id=compiling_for_linux). The 64-bit package is the one that you will need; it can compile 32-bit parts also.
- [NASM](http://www.nasm.us), the Netwide Assembler. The version provided with OS X works fine.
- Ruby (either [MRI](http://www.ruby-lang.org/) or [JRuby](http://www.jruby.org) should do). If you are using OS X, this is already included with the OS.

### Compiling:
    
    rake
    rake install

(Not all parts of the system has been converted to the rake-based compilation process yet. Feel free to send a patch/pull request if you add support for building some of the broken parts of the system.)

## Authors

This is an incomplete list of the authors of the chaos operating system, sorted alphabetically on last name. If you feel your name should be on this list but you aren't - please let us know.

* Martin Alvarez
* Henrik Hallin - Previous core developer
* John Hennessy
* [Johannes Lundberg](http://github.com/johannesl)
* [Per Lundberg](http://github.com/perlun) - Previous core developer
* Erik Moren
* Gustav Sinder
* Johan Thim - [servers/video/vga/vgalib.asm](servers/video/vga/vgalib.asm)
* Jonas Tärnström
* Noah Williamsson
* Andreas Zetterlund
* Anders Öhrt - Previous core developer

## License

(The license we use is commonly known as the BSD license, without advertising clause (also known as "3-clause"). Some  subdirectories in this directory may still contain references to the GPL since this is the license we used previously. However, the code can now be used under the BSD license - to the best of our knowledge, all major copyright holders have agreed to this license change. If you hold the copyright to some of the code herein and are in disagreement to this, please notify us immediately and we will work something out.)

The code in the following directories/files are to be treated as licensed under the [GNU GPL](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) since they are derivates of code from the Linux kernel:

* [libraries/string/inlines.h](libraries/string/inlines.h) (should be fixed).
* servers/system/pci
* servers/network/3c509
* servers/network/ne2000
* [storm/ia32/cpuid.c](storm/ia32/cpuid.c) (should be rewritten so the whole kernel can be used under the BSD license).

We encourage code that is written for chaos (servers, programs, libraries) to use the license below. This license allows us, and anyone else for that matter, to use your code freely, incorporate it in future versions of chaos and similar. However, unlike with the GNU GPL, we don't force this choice upon you. The freedom is yours. Use it wisely.

---------------------------------------------------

### chaos License (3-clause BSD)

Copyright © 1999-2000, 2006-2007, 2013 chaos development.<br/>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of chaos development nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

**THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.**

### License for storm/ia32/compiler_rt

Copyright © 2009-2013 by the contributors listed in [CREDITS.TXT](storm/ia32/compiler_rt/CREDITS.TXT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
