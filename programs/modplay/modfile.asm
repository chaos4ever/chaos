global                  modfile_axelf, modfile_axelf_size
global                  modfile_breath, modfile_breath_size
global                  modfile_chipmunk, modfile_chipmunk_size
global                  modfile_enigma, modfile_enigma_size
global                  modfile_skogen10, modfile_skogen10_size

section .data

modfile_axelf:          incbin      "AXELF.MOD"
modfile_axelf_size:     dd $-modfile_axelf

modfile_breath:         incbin      "BREATH.MOD"
modfile_breath_size:    dd $-modfile_breath

modfile_chipmunk:       incbin      "chipmunk.mod"
modfile_chipmunk_size:  dd $-modfile_chipmunk

modfile_enigma:         incbin      "Enigma.mod"
modfile_enigma_size:    dd $-modfile_enigma

modfile_skogen10:       incbin      "skogen10.mod"
modfile_skogen10_size:  dd $-modfile_skogen10
