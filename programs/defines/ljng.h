#ifndef __LJNG_H__
#define __LJNG_H__

#include <system/system.h>

#include "keydef.h"

char LJNG_Process(void);

void InitGUI();
void MessageBox (int x,int y,const char *message);
void MessageBBox (int x,int y,int w,int h);
void fillkeys(u8 *k);

extern char pixsize,bppix;
extern int msg_x,msg_y;
extern long NewMSG;

extern char badsoundbuff;
extern int MasterVolume;

extern u8 *screen,*screen2;

extern char *path_srm,*path_save,*path_roms,*path_snap;
extern char CurrentROMFile[256],roms_dir[256];

extern char cpucycle;

extern long audio_buffer_pos;
extern char buffer_filled;
extern long buffposn;

extern char viewGUI;

extern char fullscreen;
extern char doubleres;

extern u8 *keys;
extern u16 ljng_keysym[256];

extern int ShowFR,FR,FRcnt,frame;

extern char enablesound,Sound16bits,SoundStereo;
extern int buffsize;
extern long snd_frequency;
extern char InterPolateSnd;

extern char quit;

/* Joysticks. */

#if FALSE
extern char UseJoystickPl1;
extern char UseJoystickPl2;
extern SDL_Joystick *joy1,*joy2;
extern char JoystickPl1Index,JoystickPl2Index;
extern int joy1nb,joy2nb;
#endif

extern char usezapper;

#endif /* !__LJNG_H__ */
