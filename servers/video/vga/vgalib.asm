; Abstract: x86 assembly-level VGA helper functions
; Author: Johan Thim (Thank you for your great help!)
;
; © Copyright 1999 chaos development

; FIXME: Rewrite this in C!

extern          graphic_video_memory
global          vga_set_mode

section         .data

mode_80x50:     db   067h, 000h, 003h, 000h, 003h, 000h, 002h, 05Fh, 04Fh, 050h, 082h, 055h, 081h, 0BFh, 01Fh, 000h
                db   047h, 006h, 007h, 000h, 000h, 000h, 000h, 09Ch, 08Eh, 08Fh, 028h, 01Fh, 096h, 0B9h, 0A3h, 0FFh
                db   000h, 000h, 000h, 000h, 000h, 010h, 00Eh, 000h, 0FFh, 000h, 001h, 002h, 003h, 004h, 005h, 014h
                db   007h, 038h, 039h, 03Ah, 03Bh, 03Ch, 03Dh, 03Eh, 03Fh, 00Ch, 000h, 00Fh, 008h, 000h

mode_320x200x256:
                db   063h, 000h, 003h, 001h, 00Fh, 000h, 00Eh, 05Fh, 04Fh, 050h, 082h, 054h, 080h, 0BFh, 01Fh, 000h
                db   041h, 000h, 000h, 000h, 000h, 000h, 000h, 09Ch, 08Eh, 08Fh, 028h, 040h, 096h, 0B9h, 0A3h, 0FFh
                db   000h, 000h, 000h, 000h, 000h, 040h, 005h, 00Fh, 0FFh, 000h, 001h, 002h, 003h, 004h, 005h, 006h
                db   007h, 008h, 009h, 00Ah, 00Bh, 00Ch, 00Dh, 00Eh, 00Fh, 041h, 000h, 00Fh, 000h, 000h

mode_640x400x256:
                db   063h, 000h, 003h, 001h, 00Fh, 000h, 006h, 05Fh, 04Fh, 050h, 082h, 054h, 080h, 0BFh, 01Fh, 000h
                db   040h, 000h, 000h, 000h, 000h, 000h, 000h, 09Ch, 08Eh, 08Fh, 050h, 000h, 096h, 0B9h, 0E3h, 0FFh
                db   000h, 000h, 000h, 000h, 000h, 040h, 005h, 00Fh, 0FFh, 000h, 001h, 002h, 003h, 004h, 005h, 006h
                db   007h, 008h, 009h, 00Ah, 00Bh, 00Ch, 00Dh, 00Eh, 00Fh, 041h, 000h, 00Fh, 000h, 000h

mode_640x480x16:
                db   0E3h, 000h, 003h, 001h, 00Fh, 000h, 006h, 05Fh, 04Fh, 050h, 082h, 054h, 080h, 00Bh, 03Eh, 000h
                db   040h, 000h, 000h, 000h, 000h, 000h, 000h, 0EAh, 08Ch, 0DFh, 028h, 000h, 0E7h, 004h, 0E3h, 0FFh
                db   000h, 000h, 000h, 000h, 000h, 000h, 005h, 00Fh, 0FFh, 000h, 001h, 002h, 003h, 004h, 005h, 014h
                db   007h, 038h, 039h, 03Ah, 03Bh, 03Ch, 03Dh, 03Eh, 03Fh, 001h, 000h, 00Fh, 000h, 000h

mode_table:     dd      mode_320x200x256
                dd      mode_80x50
                dd      mode_640x480x16
                dd      mode_640x400x256

section         .text

vga_set_mode:   push    ebp
                mov     ebp, esp
                pusha

                mov     eax, [ebp +  8]
                mov     esi, [mode_table + eax * 4]
                push    eax
                call    setmode
                add     esp,4

                popa
                pop     ebp
                ret

; End of vga_set_mode

setmode:        mov     dx,3c2h                 ; misc addr
                lodsb
                out     dx,al

                mov     dx,3dah                 ; status addr
                lodsb
                out     dx,al

                xor     ecx,ecx
                mov     dx,3c4h                 ; seq. addr
.1:             lodsb
                xchg    al,ah
                mov     al,cl
                out     dx,ax
                inc     ecx
                cmp     cl,4
                jbe     .1

                mov     dx,3d4h                 ; crtc addr
                mov     ax,0e11h                ; clear protection
                out     dx,ax

                xor     ecx,ecx
                mov     dx,3d4h                 ; crtc addr
.2:             lodsb
                xchg    al,ah
                mov     al,cl
                out     dx,ax
                inc     ecx
                cmp     cl,18h
                jbe     .2

                xor     ecx,ecx
                mov     dx,3ceh                 ; graphics controll addr
.3:             lodsb
                xchg    al,ah
                mov     al,cl
                out     dx,ax
                inc     ecx
                cmp     cl,8
                jbe     .3

                mov     dx,3dah                 ; clear flip/flop
                in      al,dx

                xor     ecx,ecx
                mov     dx,3c0h                 ; attrcon
.4:             in      al, dx                  ; change flip/flop state
                mov     ah, al
                in      al, dx
                mov     al,cl
                out     dx,al
                lodsb
                out     dx,al
                inc     ecx
                cmp     cl,14h
                jbe     .4

                mov     al,20h
                out     dx,al

                ret
