SDLDEFINES=

CCC = gcc
CC = gcc
NASM = nasm

INCLUDES = -I/mnt/chaos/data/programming/c/headers

OPTIMISE = -O3 -mpentium -fomit-frame-pointer -fno-exceptions

CCFLAGS = $(INCLUDES) $(OPTIMISE) \
-I. \
-Ines \
-DNO_INLINE_SET_GET \
-DVAR_CYCLES \
-DCPU_SHUTDOWN \
-DSPC700_SHUTDOWN

OBJECTS += nes/unes.o nes/unes_apu.o nes/unes_ppu.o nes/unes_io.o \
	nes/unes_mapper.o nes/snss.o nes/unes_nsf.o

OBJECTS += $(objs) nes/n6502/6502.o
CCFLAGS += -Ines/n6502

OBJECTS += ljng.o

CFLAGS=$(CCFLAGS)

.SUFFIXES: .o .cpp .c .cc .h .m .i .S .asm

LDLIBS =  

all: ljng

ljng: $(OBJECTS)
	$(CCC) $(INCLUDES) -o $@ $(OBJECTS) $(LDLIBS) $(EXTRALIBS)

.cpp.o:
	$(CCC) $(INCLUDES) -c $(CCFLAGS) $*.cpp -o $@

.c.o:
	$(CC) $(INCLUDES) -c $(CCFLAGS) $*.c -o $@

.cpp.S:
	$(CCC) $(INCLUDES) -S $(CCFLAGS) $*.cpp -o $@

.cpp.i:
	$(CCC) $(INCLUDES) -E $(CCFLAGS) $*.cpp -o $@

.S.o:
	$(CCC) $(INCLUDES) -c $(CCFLAGS) $*.S -o $@

.S.i:
	$(CCC) $(INCLUDES) -c -E $(CCFLAGS) $*.S -o $@

.asm.o:
	$(NASM) -f elf -i. -i./snes -o $@ $*.asm

clean:
	rm -f $(OBJECTS) snes/offsets.o

cleanall:
	rm -f $(OBJECTS) snes/offsets.o ljng.exe

release: clean all

nes/unes.o: nes/unes.h nes/unes_apu.h nes/unes_mapper.h nes/unes_ppu.h \
	nes/unes_io.h nes/snss.h nes/font.h nes/unes_pal.h ljng.h
nes/unes_nsf.o: nes/unes.h
nes/unes_apu.o: nes/unes.h nes/unes_apu.h
nes/unes_ppu.o: nes/unes.h nes/unes_ppu.h
nes/unes_io.o: nes/unes.h nes/unes_io.h nes/unes_mapper.h nes/unes_apu.h
nes/unes_mapper.o: nes/unes.h nes/unes_mapper.h
nes/unes_loadzip.o: nes/unes.h
nes/n6502/6502.o: nes/n6502/6502.h

