global 		modfile, modfile_size
section .data

modfile:            incbin      "AXELF.MOD"

modfile_size:       dd $-modfile
