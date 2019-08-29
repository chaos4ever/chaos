global                  modfile_0, modfile_0_size
global                  modfile_1, modfile_1_size
global                  modfile_2, modfile_2_size
global                  modfile_3, modfile_3_size
global                  modfile_4, modfile_4_size
global                  modfile_5, modfile_5_size
global                  modfile_6, modfile_6_size
global                  modfile_7, modfile_7_size
global                  modfile_8, modfile_8_size
global                  modfile_9, modfile_9_size

section .data

modfile_0:              incbin      "mods/AXELF.MOD"
modfile_0_size:         dd $-modfile_0

modfile_1:              incbin      "mods/BREATH.MOD"
modfile_1_size:         dd $-modfile_1

modfile_2:              incbin      "mods/chipmunk.mod"
modfile_2_size:         dd $-modfile_2

modfile_3:              incbin      "mods/Enigma.mod"
modfile_3_size:         dd $-modfile_3

modfile_4:              incbin      "mods/skogen10.mod"
modfile_4_size:         dd $-modfile_4

modfile_5:              incbin      "mods/space_debris.mod"
modfile_5_size:         dd $-modfile_5

modfile_6:              incbin      "mods/GSLINGER.MOD"
modfile_6_size:         dd $-modfile_6

modfile_7:              incbin      "mods/ELYSIUM.MOD"
modfile_7_size:         dd $-modfile_7

modfile_8:              incbin      "mods/stardstm.mod"
modfile_8_size:         dd $-modfile_8

modfile_9:              incbin      "mods/ASM94.MOD"
modfile_9_size:         dd $-modfile_9
