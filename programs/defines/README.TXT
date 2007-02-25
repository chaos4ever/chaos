-----------------------------------+
Little John New Generation 0.1.1
----------------------------------->
Date : 3rd June 2000.
Author : Yoyo.

----------------------------------+
Location : ljohn.romulation.net (main) / www.altern.org/ymagnien
Webmaster : Pazoo
---------------------------------->

-----------+
What is it ?
----------->

  This program is an opensource NES/SNES emulator. It is based on 
Simple Direct Media Layer (SDL), a great library from Sam Lantiga.
  It allows you to access video, sound, joysticks, timers on many platforms.
  Little John uses Neil Corlett 6502 emulator with minor modifications and 
the 6502 emulator package from Marat Fayzullin.
  Current binaries are compiled with Neil Corlett CPU core, which is more compatible.

  Little John is still in development stages, but it can run quiet well a lot of
classic nes roms. Remember that you have to own the roms to use them.

  Snes emulation uses Snes9X.
  NES Sound emulation is based on sources from Matthew Conte, author of a nsf
player, Nosefart. 

  NES and SNES are a trademark of Nintendo.
  
--------------+
What you need ?
-------------->

  A fast computer, it is mainly C, not very optimized. My aim is portability, not
the fastest emu ever made.
  It runs just fine on my Celeron 466, with 44Khz 16bits stereo sound,
filtering enabled, in fullscreen.
  I havent tested it on slower computer, but I think it should run on P133Mhz
with a little frameskip and 22Khz 8 bits mono sound.

---------------+
How to install ?
--------------->

  You can download precompiled version for Linux(intel), Windows(intel), BeOS (intel, R5)
and SunSparc/Solaris.
  Except the Windows&BeOS versions, binaries come without the SDL libraries. Download
them on the SDL page, www.devolution.com/~slouken/SDL.
  Little John uses the 1.1 version, but it should run with 1.xx versions.

  If you want to compile the sources by yourself, you will have to use the Makefile
 and probably edit the 6502 emulator header.
  I advice you to use Marat Fayzullin cpu core, becoz it can run on high-endian
cpu. But if you have a low-endian one, use Neil Corlett one (#marat=1 in 
Makefile). Some games will run only on Neil cpu core (BattleToads,...).

  Be sure to have the latest SDL installed and check the m6502/M6502.h file 
if you want to compile with Marat's cpu core on high endian cpu.

  If you want to port Little John on a new platform, please inform me. 

-----------------------------+
What does it provide for now ?
----------------------------->

  *- Mappers 0,1,2,3,4,7,9 emulation (with some bugs sometimes,...).
  *- Still some minor graphic bugs in some games. 
  *- Sound : 5 channels emulated, near perfectly, thanks to Matthew Conte.
  *- Joypads : 2 joypads emulated on keyboard or joystick (min 2 buttons).
  *- Zapper support through mouse.
  *- Savestate : Snss format(save/load), Nesticle(load) for the NES, Snes9x own
format for the Snes.
  *- SRAM : saved on disk.
  *- A lot of platform : Unix, Linux, Windows, BeOS and more ...
  *- ZIP Rom support with Zlib.
  *- mono/stereo 8/16 bits sound emulation, filtering supported.
  *- Auto/manual frameskip.
  *- Nesticle save state can be loaded, be sure to use file which have the 
same name as the game with .sta, .st1 -> .st6 extension or it wont work.
  *- basic GUI interface
  *- config file
  *- NSF integrated player.

----------------+
What is planned ?
---------------->  

  *- More platforms for the precompiled versions.
  *- Full sound emulation.
  *- More mappers.
  *- PPU 2005,6,7 registers bugfixes.
  *- New graphic modes, with the help of OpenGL (supported in SDL since
v1.1.1).
  *- Zsnes save state compatibility.
  *- Input redefinition within GUI.  
  
--------------+
How to use it ?
-------------->  

   Usage --> ljng.exe [romfile] [Options]

  Type 'ljng.exe' to have options list.
  Use F1 during emulation to list other functions.
  If you want to reset config, delete the ljohn.cfg file.

  Default keys are : 

  #order is A,B,X,Y,L,R,START,SELECT,UP,DOWN,LEFT,RIGHT

   Pl1 : x,c,s,d,z,e,enter,space & arrows

   Pl2 : del,end,ins,home,pageup,pagedown,kp_enter,kp_plus,kp8,kp5,kp4,kp6

  To exit Nes emulation, press SHIFT+ESC
  To exit Snes emulation, press ESC

  Inline help (F1) isn't available when using the nsf player.

  
-------------+
Known problems
------------->

  Some games have incorrect mapper numbers, so you will have to force to the
good value. Usual values are 1,2,3,4. 
  You can use GoodNES, download it at www.vintagegaming.com (section NES).
  Still some bugs in PPU and mappers (especially mapper 4, Radracer 2 has very
corrupted graphics).

--------+
Thanks to
-------->

  Pazoo for testing, support and ideas.  
	*- www.ensicaen.ismra.fr/~villemin -*
  The BOSS for believing in us. Check out his great emulation resource,
  	*- www.romulation.net -*
  Sam Lantiga for SDL and his support.
  	*- www.devolution.com/~slouken -*
  Yohsi for his great nes documentation and the parodius network.
  	*- nesdev.parodius.net -*
  Marat Fayzullin for his m6502 package
  	*- www.komkon.org/fms -*
  Darcnes & Tuxnes authors for publishing their sources.
        *- get them at www.zophar.net or www.vintagegaming.com -*
  Loopy for the nes palette.
        *- nesdev.parodius.com -*
  Matthew Conte for the sound emulation.
	*- www.nofrendo.org -*
  Neil Corlett for the cpu core.
  NesTEN authors for giving sources of their mappers implementation.
        *- check zophar or vintagegaming to download the latest version *-
  Snes9X team for their great emulator.
        *- www.snes9x.com *-
  Aaron Holmes for helping me fixing bugs.
