# README

<img src="https://raw.githubusercontent.com/chaos4ever/chaos/master/misc/chaos-logo.png" width="500">

[![Build Status](https://travis-ci.org/chaos4ever/chaos.svg?branch=master)](https://travis-ci.org/chaos4ever/chaos)

An operating system for x86, written mostly in C with minor parts in assembly language. For more details on the background to the project and the full history etc, please see [our web site](http://chaosdev.io).

## Quick start

**Note**: If you just want to try things out, go straight to the [Releases](https://github.com/chaos4ever/chaos/releases) page and download an `.iso` file that you can run in whichever VM software you like (VirtualBox, Parallels, VMware, etc).

To build the system from source, use the following approach. (Requires Vagrant and VirtualBox to be pre-installed.)

```shell
$ git clone ... && cd chaos
$ vagrant up
$ vagrant ssh
$ rake
$ rake install
```

If all goes well, this should give you an `.iso` file as output. For more details, consult [our web site](http://chaosdev.io).

## License

### General

(The license we use is commonly known as the BSD license, without advertising clause (also known as "3-clause"). Some  subdirectories in this directory may still contain references to the GPL since this is the license we used previously. However, the code can now be used under the BSD license - to the best of our knowledge, all major copyright holders have agreed to this license change. If you hold the copyright to some of the code herein and are in disagreement to this, please notify us immediately and we will work something out.)

The code in the following directories/files are to be treated as licensed under the [GNU GPL](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) since they are derivates of code from the Linux kernel:

* [libraries/string/inlines.h](libraries/string/inlines.h) (should be fixed).
* servers/system/pci
* [storm/x86/cpuid.c](storm/x86/cpuid.c) (should be rewritten so the whole kernel can be used under the BSD license).

We encourage code that is written for chaos (servers, programs, libraries) to use the license below. This license allows us, and anyone else for that matter, to use your code freely, incorporate it in future versions of chaos and similar. However, unlike with the GNU GPL, we don't force this choice upon you. The freedom is yours. Use it wisely.

---------------------------------------------------

### chaos License (3-clause BSD)

Copyright © 1999-2000, 2006-2007, 2013, 2015-2017 chaos development.<br/>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of chaos development nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

**THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.**

### License for storm/x86/compiler_rt

Copyright © 2009-2013 by the contributors listed in [CREDITS.TXT](storm/x86/compiler_rt/CREDITS.TXT)

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
