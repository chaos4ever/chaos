# README

<img src="https://raw.githubusercontent.com/chaos4ever/chaos/master/misc/chaos-logo.png" width="500">

[![Build Status](https://travis-ci.org/chaos4ever/chaos.svg?branch=master)](https://travis-ci.org/chaos4ever/chaos)

Welcome to chaos. chaos is an operating system developed around the year 2000 for the x86 platform (also referred to as "ia32" in the codebase). It contains working protected mode/virtual memory initialization, processes and threads, interrupt support, PCI, text-based VGA and a simple IPv4 stack (supporting UDP, ARP and ICMP). Naturally, given the hobbyist nature of the project, none of the features we just mentioned is "complete" in the sense that it supports all features of the PCI bus, or all features of UDP, ICMP etc. Nonetheless, the system was at an earlier point stable to the point where it possible to e.g. use it for network bridging - you could run e.g. ssh "through" a machine running chaos. It would also reply to ICMP echo (i.e. "ping") packets, as well as support outgoing ping packets to other machines.

Having that said, there are a number of issues with the codebase which made us start the development of the new [stormG3](https://github.com/chaos4ever/stormG3) kernel (which would replace the existing storm "G1" kernel. There was also a [stormG2](https://github.com/chaos4ever/stormG2) project being undertaken but it was abandoned before being completed). The most critical issue with stormG1 is that the kernel leaks memory; the number of free pages decrease gradually over time. Because of this and the general feeling that the system would benefit from a major refactoring (hey, isn't it always fun to rewrite your code, trying to get things better done the second time? :wink:) the storm G3 project was initiated, where we attempted to Do Things Right&trade; to a greater point than had been earlier done. Like many hobby/spare-time projects, the project was never finished and storm G3 is therefore currently in a very unfinished state. Nonetheless, feel free to check it out if you like.

## Compiling chaos

**Note**: If you just want to try things out, go straight to the [Releases](https://github.com/chaos4ever/chaos/releases) page and download an `.iso` file. Then, skip down to "Running chaos" below.

### Prerequisites:

- Linux, Mac OSX or Windows.
- [Vagrant](https://www.vagrantup.com/). If you don't know what this is, go check out their web page. It makes things like this much easier and smoother.
- [VirtualBox](https://www.virtualbox.org/)
- If Windows: an `ssh` client. (Install [mingw-get](http://www.mingw.org/wiki/getting_started) and then select at least the `msys-openssh`, `msys-base` and `msys-bash` packages.)

### Setting up the dev environment

* Fork the repo (using the Fork button in the upper right).
* Clone your fork (using the clone button here at GitHub or straight in the command line).

#### On Linux or Mac

* Open a terminal program.
* Change working directory to your working copy (`cd ~/git/chaos` or similar)
* Run the following commands:

        vagrant up
        vagrant ssh

#### On Windows

* Launch `msys.bat` from the MSYS packages, as Administrator. **Note**: You *must* run `msys.bat` as Administrator. This is because symbolic links with VirtualBox is only supported when the VM is launched as an Administrator, and the `chaos` build process relies on symbolic links. Typical errors you can get if you do not do this right is "Protocol error" and similar. If this happens, do a `vagrant halt` and then retry the commands below in an admin `msys.bat` instead.)
* Change working directory to your working copy (`cd /c/git/chaos`, assuming that you have the code in `c:\git\chaos`) and run the following commands:

        vagrant up
        vagrant ssh

### Compiling:

    rake
    rake install

(Not all parts of the system has been converted to the `rake`-based compilation process yet, but we're working on it. Feel free to send a patch/pull request if you add support for building some of the broken parts of the system.)

This should create a `chaos.iso`, if all worked correctly. Congratulations! You're just one step away from having a fully working version of chaos running now. :)

### Running chaos

* Fire up your favorite VM software (VirtualBox, Parallels, VMware Player/Fusion, etc. Even Virtual PC will probably do).
* Create a tiny VM (256 megs of RAM will do, no hard drive etc) with the `chaos.iso` (or `chaos-0.0.x.iso` if you downloaded it from here) file as the CD drive.
* Launch it. If all went well, you should get an image like this:

![chaos 0.0.3 in action](https://raw.githubusercontent.com/chaos4ever/chaos/master/misc/chaos-0.0.3.png)

From there, just follow the instructions on the screen.

Want to help us out? Feel like contributing? Fork the project and start hacking away! :smile: The easiest way to get in touch with is this GitHub page, for the moment. We may in the future put back our mailing lists and stuff, for public communication, but for now, make a fork and submit pull requests, create issues if you find issues that you think should be fixed, etc. **Don't be afraid**, we usually don't bite. :wink:

## Authors

This is an incomplete list of the authors of the chaos operating system, sorted alphabetically on last name. If you feel your name should be on this list but you aren't - please let us know.

* Martin Alvarez
* [Henrik Hallin](http://github.com/CaspecoHenrik) - Previous core developer
* John Hennessy
* [Johannes Lundberg](http://github.com/johannesl)
* [Per Lundberg](http://github.com/perlun) - Previous core developer
* Erik Moren
* Gustav Sinder
* Johan Thim - [servers/video/vga/vgalib.asm](servers/video/vga/vgalib.asm)
* Jonas Tärnström - did some work on the [video library](libraries/video) which was unfortunately never completed
* Noah Williamsson
* Andreas Zetterlund
* Anders Öhrt - Previous core developer

## License

### General

(The license we use is commonly known as the BSD license, without advertising clause (also known as "3-clause"). Some  subdirectories in this directory may still contain references to the GPL since this is the license we used previously. However, the code can now be used under the BSD license - to the best of our knowledge, all major copyright holders have agreed to this license change. If you hold the copyright to some of the code herein and are in disagreement to this, please notify us immediately and we will work something out.)

The code in the following directories/files are to be treated as licensed under the [GNU GPL](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) since they are derivates of code from the Linux kernel:

* [libraries/string/inlines.h](libraries/string/inlines.h) (should be fixed).
* servers/system/pci
* [storm/ia32/cpuid.c](storm/ia32/cpuid.c) (should be rewritten so the whole kernel can be used under the BSD license).

We encourage code that is written for chaos (servers, programs, libraries) to use the license below. This license allows us, and anyone else for that matter, to use your code freely, incorporate it in future versions of chaos and similar. However, unlike with the GNU GPL, we don't force this choice upon you. The freedom is yours. Use it wisely.

---------------------------------------------------

### chaos License (3-clause BSD)

Copyright © 1999-2000, 2006-2007, 2013, 2015 chaos development.<br/>
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
