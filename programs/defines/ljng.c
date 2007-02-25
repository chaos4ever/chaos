/**********************************************************/
/*                                                        */
/*        Little John New Generation  v0.1.1              */
/*        ----------------------------------              */
/*                                                        */
/*        (c) 2000 Yohann Magnien                         */
/*        (c) 2000 Snes9x TEAM                            */
/*                                                        */
/*     Tested systems :                                   */
/*       Linux (intel) (gcc, nasm)                        */
/*       Windows 95,98,NT (intel)  (cygwin's gcc, nasm)   */
/*       BeOS r5 (intel) (gnupro's gcc, nasm)             */
/*       Unix (SunSparc) (gcc)                            */
/*                                                        */
/*     Supported by SDL, but not tested                   */
/*       Irix, MacOS, ... check out the SDL website       */
/*                                                        */
/*        Nes emulation by Yohann Magnien                 */
/*        Snes emulation by Snes9x TEAM                   */
/*                                                        */
/*       This software is under the GPL License           */
/*                                                        */
/**********************************************************/

#include "keydef.h"

#include "unes.h"
#include "font.h"

#include "port.h"
#include "snes9x.h"
#include "soundux.h"

char *path_srm="./srm/";
char *path_save="./save/";
char *path_roms="./";
char *path_snap="./snap/";
char roms_dir[256],CurrentROMFile[256];

char cpucycle;
char badsoundbuff;
char viewGUI=0;

byte nesemu;

int MasterVolume;
long snd_frequency;
char InterPolateSnd;
long audio_buffer_pos;
byte buffer_filled;
long buffposn;
byte enablesound,Sound16bits,SoundStereo;
int buffsize;
char filechoosed;

char fullscreen,doubleres;

char quit;


int ShowFR,FR,FRcnt,frame;

char usezapper;

Uint8 *keys;


SDL_Surface *screen,*screen2;
char pixsize;
char bppix;
int msg_x,msg_y;
long NewMSG;


// Joysticks
char UseJoystickPl1;
char UseJoystickPl2;
SDL_Joystick *joy1,*joy2;
char JoystickPl1Index,JoystickPl2Index;
int joy1nb,joy2nb;

Uint16 ljng_keysym[256];

byte printcol=32;
word printcol16=(31<<11)|(63<<5)|31;

void Yo_Print(Uint16 x,Uint16 y,char c)
{
  int indice=c-32; //index in the font.h array
  int indicex,indicey;
  Uint16 i,j;

  if (indice>=96) indice=0;
  if (indice<0) indice=0;
  indicey=(indice>>4)*9;
  indicex=(indice&0xf)<<3;

  if (!bppix)
    {
      if (pixsize)
	{
	  for (i=0;i<8;i++)
	    for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint8 *)(screen->pixels))[((y+j)<<9)+i+x]=printcol;
	}
      else
	{
	  for (i=0;i<8;i++)
	    for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint8 *)(screen->pixels))[((y+j)<<8)+i+x]=printcol;
	}
    }
  else
    {
      if (pixsize)
	{
	  for (i=0;i<8;i++)
	    for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint16 *)(screen->pixels))[((y+j)<<9)+i+x]=printcol16;
	}
      else
	{
	  for (i=0;i<8;i++)
	    for (j=0;j<9;j++) if (font[j+indicey][i+indicex]=='#') ((Uint16 *)(screen->pixels))[((y+j)<<8)+i+x]=printcol16;
	}
    }
}

void Messageb (const char *message)
{
  int l;
  int k;
  l=strlen(message);
  k=0;
  NewMSG=30*(1+(l>>4));
  while (k<l)
    {
      if (message[k]!='\n') 
	{
	  Yo_Print(msg_x,msg_y,message[k]);
	  msg_x+=6;
	}
      else 
	{
	  msg_y+=8;
	  msg_x=0;
	}
      
      if (msg_x>=(pixsize?512-6:256-6))
	{
	  msg_x=0;
	  msg_y+=8;
	}
      if (msg_y>=(pixsize?480-15:240-8))
	{
	  msg_x=0;
	  msg_y=0;
	}
      k++;
    }
}

void MessageBox (int x,int y,const char *message)
{
  int l;
  int k;
  int xx,yy,ox;
  int w,h,i;
  l=strlen(message);
  
  w=0;
  i=0;
  h=10;
  for (k=0;k<l;k++)
    {
      if (message[k]=='\n') 
	{
	  h+=8;
	  if (w<i) w=i;
	  i=0;
	}  
      i+=6;
    }
  if (w<i) w=i;
  w+=2;

  k=0;
  ox=x+1;
  if (bppix)
    for (yy=y;yy<y+h;yy++)
    for (xx=x;xx<x+w;xx++) 
      {
	if (yy==y) 
	  ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(31<<11);
	else
	  if (xx==x) 
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(31<<11);
	else
	  if (yy==(y+h-1))
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(10<<11);
	else
	  if (xx==(x+w-1))
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(10<<11);
	else
	  ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(20<<11);
      }
    else
  for (yy=y;yy<y+h;yy++)
    for (xx=x;xx<x+w;xx++) 
      {
	if (yy==y) 
	  ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=38;
	else
	  if (xx==x) 
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=38;
	else
	  if (yy==(y+h-1))
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=7;
	else
	  if (xx==(x+w-1))
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=7;
	else
	  ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=23;
      }	  

  x++;
  y++;
  while (k<l)
    {
      if (message[k]!='\n') 
	{
	  Yo_Print(x,y,message[k]);
	  x+=6;
	}
      else
	{
	  x=ox;
	  y+=8;
	}
      k++;
    }
}


void MessageBBox (int x,int y,int w,int h)
{

  int xx,yy;

  if (bppix)
    for (yy=y;yy<y+h;yy++)
    for (xx=x;xx<x+w;xx++) 
      {
	if (yy==y) 
	  ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(31<<11)|(10<<5);
	else
	  if (xx==x) 
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(31<<11)|(10<<5);
	else
	  if (yy==(y+h-1))
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(10<<11)|(2<<5);
	else
	  if (xx==(x+w-1))
	    ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=(10<<11)|(2<<5);
	else
	  ((word *)(screen->pixels))[(yy<<(8+pixsize))+xx]=0;
      }
    else
  for (yy=y;yy<y+h;yy++)
    for (xx=x;xx<x+w;xx++) 
      {
	if (yy==y) 
	  ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=38;
	else
	  if (xx==x) 
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=38;
	else
	  if (yy==(y+h-1))
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=7;
	else
	  if (xx==(x+w-1))
	    ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=7;
	else
	  ((byte *)(screen->pixels))[(yy<<(8+pixsize))+xx]=13;
      }	  
}

void InitGUI()
{
  msg_x=msg_y=0;

  memset((screen->pixels),(bppix?0:13),256*16<<(pixsize+bppix));
  MessageBox(0,0,"File");
  MessageBox(28,0,"Options");
  MessageBox(28+46,0,"Video");
  MessageBox(28+46+34,0,"Sound");
  MessageBox(28+46+34+34,0,"Controls");
  MessageBox(255-33,0,"Help");

  msg_x=NewMSG=0;
  msg_y=16;
}

byte GetDirName(char *curdir,char ***filenames,char ***dirnames,int *files, int *dirs)
{
  struct dirent *tmp;
  char **namelist,*tmpname;
  int n;
  int i,j,k;
  char tchaine[256];
  DIR *mydir;
  printf("opening : %s\n",curdir);
  mydir = opendir(curdir);
  if (mydir)
    {
      namelist=(char **)malloc(4096*sizeof(char *));
      if (!namelist)
	{
	  printf("Not enoung memory to get filelist\n");
	  exit(1);
	}
      n=0;
      i=1;
      while ((n<4096)&&i)
	{
	  tmp=readdir(mydir);
	  if (tmp)
	    {
#ifdef __CYGWIN32__  //BIG BUG!!!
		  namelist[n]=(char *)malloc(strlen(tmp->d_name+2)+1);
#else
	      namelist[n]=(char *)malloc(strlen(tmp->d_name)+1);
#endif
	      if (!namelist[n])
		{
		  printf("Malloc error while getting tmp->d_name\n");
		  exit(1);
		}
#ifdef __CYGWIN32__  //BIG BUG!!!
	      strcpy(namelist[n],tmp->d_name+2);
#else
		  strcpy(namelist[n],tmp->d_name);
#endif

	      n++;
	    }
	  else i=0;
	}
      closedir(mydir);

    }
  else n=-1;

  if (n < 0)
    {
      //      perror("scandir");
      
      if (strcmp(roms_dir,"./"))
	{
	  strcpy(roms_dir,"./");
	  return(2);
	}
      return(1);
    }
  else
    {
      
      *files=*dirs=0;
      j=0;
      i=0;
      while (j<n)
	{	  
	  strcpy(tchaine,curdir);
	  strcat(tchaine,namelist[i]);
	  if (opendir(tchaine))
	    {
	      (*dirs)++;
	      tmpname=namelist[i];
	      for (k=i;k<(n-1);k++)
		namelist[k]=namelist[k+1];
	      namelist[n-1]=tmpname;
	    }
	  else
	    {
	      (*files)++;
	      i++;
	    }
	  j++;
	}
      
      
      (*filenames)=(char **)malloc((*files)*sizeof(char *));
      (*dirnames)=(char **)malloc((*dirs)*sizeof(char *));
      
      for (i=0;i<(*files);i++)
	for (j=0;j<(*files)-1;j++)
	  
	  if (strcasecmp(namelist[j+1],namelist[j])<0)
	    {
	      tmpname=namelist[j];
	      namelist[j]=namelist[j+1];
	      namelist[j+1]=tmpname;
	    }
      


	for (i=(*files);i<n;i++)
	  for (j=(*files);j<n-1;j++)
	    if (strcasecmp(namelist[j],namelist[j+1])>0)
	      {
		tmpname=namelist[j];
		namelist[j]=namelist[j+1];
		namelist[j+1]=tmpname;
	      }
	
	k=0;
	for (i=0;i<(*files);i++)
	  {
	    if (strstr(namelist[i],".nes")||strstr(namelist[i],".NES")||strstr(namelist[i],".zip")||strstr(namelist[i],".ZIP")||strstr(namelist[i],".nsf")||strstr(namelist[i],".NSF")||strstr(namelist[i],".smc")||strstr(namelist[i],".SMC")||strstr(namelist[i],".sfc")||strstr(namelist[i],".SFC")||strstr(namelist[i],".FIG")||strstr(namelist[i],".fig"))
	      (*filenames)[k++]=namelist[i];
	    else
	      free(namelist[i]);
	      }
	for (i=0;i<(*dirs);i++)
	  {
	    (*dirnames)[i]=namelist[(*files)+i];
	  }
	
	(*files)=k;

    }
  return(0);
}

byte Reload_rom()
{
  int i,j,k,l,m;
  int files,dirs;
  char **filenames;
  char **dirnames;
  int x,y;
  int dirpos,filepos;
  char tchaine[20];
  long ltime;


  byte bouton;
  SDL_Event event;

  tchaine[19]=0;
  i=GetDirName(roms_dir,&filenames,&dirnames,&files,&dirs);
  if (i)
    {
      if ((i==2)&&GetDirName(roms_dir,&filenames,&dirnames,&files,&dirs))
	return 1;
      if (i==1) return 1;
    }

  if (bppix)
    {
      for (i=(16*256<<pixsize);i<(256*240<<pixsize);i++)
	((Uint16 *)screen->pixels)[i]=(20<<11);
    }
  else
    memset((byte *)screen->pixels+(13*256<<(pixsize)),13,(227*256<<(pixsize)));
  MessageBBox(0,61,120,170);
  MessageBBox(130,21,120,210);

  MessageBox(0,16,"Use Buttons 2&3\nTo scroll list\nClick here to exit");
  
  dirpos=-1;
  filepos=-1;
  i=j=0;
  bouton=0;

  while (!bouton)
    { //test
      SDL_PumpEvents();
      while (SDL_PollEvent(&event))
	{
	  if (event.type==SDL_QUIT) return 1;
	  if (event.type==SDL_KEYUP)
	    {
	      if (event.key.keysym.sym==SDLK_ESCAPE) return 1;
	    }
	}

      if ((x<120)&&(y>60)&&(dirpos==i))
	{
	  k=(y-62)/8;
	  if ((k<21)&&(k+dirpos<dirs))
	    {
	      k+=dirpos;
	      msg_x=2;
	      msg_y=62+(k-dirpos)*8;
	      
	      if (strlen(dirnames[k])>19) 
		{
		  memcpy(tchaine,dirnames[k],19);
		  Messageb(tchaine);
		}
	      else
	       Messageb(dirnames[k]);	      


	    }
	}

      if ((x>120)&&(y>20)&&(filepos==j))
	{
	  k=(y-22)/8;
	  if ((k<26)&&(k+filepos<files))
	    {
	      k+=filepos;
	      msg_x=132;
	      msg_y=22+(k-filepos)*8;
	      
	      if (strlen(filenames[k])>19) 
		{
		  memcpy(tchaine,filenames[k],19);
		  Messageb(tchaine);
		}
	      else
	       Messageb(filenames[k]);	      


	    }
	}


      bouton=SDL_GetMouseState(&x,&y);


      if ((x<120)&&(y>60)&&(dirpos==i))
	{
	  k=(y-62)/8;
	  if ((k<21)&&(k+dirpos<dirs))
	    {
	      k+=dirpos;
	      msg_x=2;
	      msg_y=62+(k-dirpos)*8;
	      printcol=33;
	      printcol16=(31<<11)|(63<<5);
	      
	      if (strlen(dirnames[k])>19) 
		{
		  memcpy(tchaine,dirnames[k],19);
		  Messageb(tchaine);
		}
	      else
	       Messageb(dirnames[k]);	      

	      printcol=32;
	      printcol16=(31<<11)|(63<<5)|(31);

	    }
	}

      if ((x>120)&&(y>20)&&(filepos==j))
	{
	  k=(y-22)/8;
	  if ((k<26)&&(k+filepos<files))
	    {
	      k+=filepos;
	      msg_x=132;
	      msg_y=22+(k-filepos)*8;

	      printcol=33;
	      printcol16=(31<<11)|(63<<5);
	      
	      if (strlen(filenames[k])>19) 
		{
		  memcpy(tchaine,filenames[k],19);
		  Messageb(tchaine);
		}
	      else
	       Messageb(filenames[k]);	      

	      printcol=32;
	      printcol16=(31<<11)|(63<<5)|31;

	    }
	}


      SDL_UpdateRect(screen,0,0,0,0);


      if (bouton&4)
	{
	  if ((x<120)&&(y>60))
	    {
	      if ((dirpos+5)<dirs) i+=1;
	      if ((dirpos+5)>=dirs) i=0;
	      MessageBBox(0,60,120,170);
	    }
	  if ((x>120)&&(y>20))
	    {
	      if ((filepos+5)<files) j+=1;
	      if ((filepos+5)>=files) j=0;
	      MessageBBox(130,20,120,210);
	    }
	  ltime=SDL_GetTicks();
	  while (SDL_GetTicks()-ltime<10) bouton=0;
	}

      if (bouton&2)
	{
	  if ((x<120)&&(y>60))
	    {
	      if (dirpos>0) 
		{
		  i-=1;
		  MessageBBox(0,60,120,170);
		}
	      else i=0;

	    }
	  if ((x>120)&&(y>20))
	    {
	      if (filepos>0) 
		{
		  j-=1;
		  MessageBBox(130,20,120,210);
		}
	      else j=0;

	    }

	  ltime=SDL_GetTicks();
	  while (SDL_GetTicks()-ltime<10) bouton=0;
	}
      
      if (i!=dirpos)
	{
	  dirpos=i;
	  for (k=dirpos;(k<dirs)&&((k-dirpos)<21);k++)
	    {
	      msg_x=2;
	      msg_y=62+(k-dirpos)*8;
	      
	      if (strlen(dirnames[k])>19) 
		{
		  memcpy(tchaine,dirnames[k],19);
		  Messageb(tchaine);
		}
	      else
	       Messageb(dirnames[k]);
	    }
	  SDL_UpdateRect(screen,0,0,0,0);
	}
      
      if (j!=filepos)
	{
	  filepos=j;
	  for (k=filepos;(k<files)&&((k-filepos)<26);k++)
	    {
	      msg_x=132;
	      msg_y=22+(k-filepos)*8;
	      if (strlen(filenames[k])>19) 
		{
		  memcpy(tchaine,filenames[k],19);
		  Messageb(tchaine);
		}
	      else
		Messageb(filenames[k]);
	    }
	  SDL_UpdateRect(screen,0,0,0,0);
	}
      
      if (bouton)
	{
	  if (x<120)
	    {
	      if ((y<60)||(y>229)) return 1;
	      k=(y-62)/8 + dirpos;
	      if (k>=dirs) bouton=0;
	      else 
		{
		  if (strcmp(dirnames[k],"."))
		    {
		      l=strlen(roms_dir);
		      if (roms_dir[l-2]=='.') l=1;
		      else 
			l=0;

		      if (strcmp(dirnames[k],"..")||l)
			{
			  strcat(roms_dir,dirnames[k]);
			  strcat(roms_dir,"/");
			}
		      else
			{
			  l=m=0;
			  
			  while (m<strlen(roms_dir)-1)
			    {
			      if (roms_dir[m]=='/') l=m;
			      m++;
			    }
			  roms_dir[l+1]=0;
			}
		    }
		  free(dirnames);
		  free(filenames);
		  if (GetDirName(roms_dir,&filenames,&dirnames,&files,&dirs))
		    return 1;
		  MessageBBox(0,60,120,170);
		  MessageBBox(130,20,120,210);
		  filepos=-1;
		  dirpos=-1;
		  i=j=0;
		  bouton=0;

		  while (bouton=SDL_GetMouseState(&x,&y)) SDL_PumpEvents();

		}
	    }
	  else
	    if (x<130) return 1;
	    else
	      if (x>249) return 1;
	      else
		{
		  if ((y<20)||(y>229)) return 1;
		  k=(y-22)/8 + filepos;
		  if (k>=files) bouton=0;
		}
	}
      
    }      
  while (SDL_GetMouseState(&x,&y)) SDL_PumpEvents();

  strcpy(CurrentROMFile,roms_dir);
  strcat(CurrentROMFile,filenames[k]);

  filechoosed=1;
  
  free(filenames);
  free(dirnames);  

  return 0;
}


char LJNG_Process()
{
  SDL_Event event;
  int x,y;
  char bouton;
  static int mouseview=0;
  static int temporize=0;

  SDL_PumpEvents();
  keys=SDL_GetKeyState(NULL);

  if (!mouseview) SDL_ShowCursor(usezapper);
  while (SDL_PollEvent(&event))
    {  
      switch (event.type)
	{ 
	case SDL_QUIT:exit(1);
	case SDL_MOUSEMOTION:mouseview=60; SDL_ShowCursor(1); break;
	}
    } 
		
  if (mouseview)
    {
      mouseview--;
      if ((!mouseview)&&(!usezapper)) SDL_ShowCursor(0);
    }

  bouton=SDL_GetMouseState(&x,&y);
  if (y<16)
    {
      InitGUI();
      SDL_ShowCursor(1);
      SDL_UpdateRect(screen,0,0,0,0);
      viewGUI=1;
      if ((bouton==1)&&(y<10))
	{
	  SDL_PauseAudio(1);

	  while (SDL_GetMouseState(&x,&y)) SDL_PumpEvents();

	  if (x<28)
	    { //File has been clicked
	      MessageBox(0,13,"Reset");
	      MessageBox(0,25,"Load File");
	      MessageBox(0,37,"Exit");

	      SDL_UpdateRect(screen,0,0,0,0);

	      bouton=0;
	      while (!SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
	      while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);

	      if (y>13)
		{
		  if ((y<25)&&(x<32))
		    {
		      if (nesemu) Reset();
		      else S9xReset();
		    }
		  else
		    if ((y<37)&&(x<56)) 
		      {
			bouton=Reload_rom();
			memset(screen->pixels,(bppix?0:13),(screen->w)*(screen->h)<<(bppix));
			if ((SoundMode!=3)&&(enablesound)) 
			  {
			    SDL_PauseAudio(0);
			    audio_buffer_pos=0;
			    buffposn=0;
			    buffer_filled=0;
			  }
			return(bouton);
		      }
		    else
		      if ((y<49)&&(x<26))
			{quit=1; return 0;}
		}
	    }
	  else
	    if (x<28+46)
	      { //Options
		while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
		Message("Not yet!\nUse command line\n",1);
		NewMSG=120;
	      }
	    else
	      if (x<28+46+34)
		{ //Video
		  while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);

		  MessageBox(28+46,13,(fullscreen?"windowed":"fullscreen"));
		  MessageBox(28+46,25,(doubleres?"Zoom 1:1":"Zoom 2:1"));
		  MessageBox(28+46,37,"Frameskip");
		  MessageBox(28+46,49,(ShowFR?"Hide framerate":"Show Framerate"));
		  SDL_UpdateRect(screen,0,0,0,0);

		  while (!SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
		  while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);

		  if ((y>13)&&(x>28+46))
		    {
		      if ((y<25)&&(x<28+46+54))
			{
			  if (SDL_WM_ToggleFullScreen(SDL_GetVideoSurface()))
				fullscreen^=1;
			}
		      else
			if ((y<37)&&(x<28+46+48))
			  {
			    //			    switch doubleres
			    SDL_FreeSurface(screen2);
			    SDL_FreeSurface(screen);
			    if (nesemu) free(Vbuffer);
			    else S9xDeinitDisplay();
			    doubleres^=1;
			    pixsize^=1;
			    if (nesemu) InitDisplay();
			    else S9xInitDisplay(0,NULL);
			    InitGUI();

			  }
		      else
			if ((y<49)&&(x<28+46+54))
			  {
			    // change frameskip
			    Message("Not yet!\nUse command line\n",1);
			    NewMSG=120;
			  }
			else
			if ((y<61)&&(x<28+46+84))
			  {
			    // show/hide framerate
			    ShowFR^=1;
			  }
		    }


		}
	      else
		if (x<28+46+34+34)
		  { //Sound
		    while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);

		    if (!enablesound) 
		      MessageBox(28+46+34,13,"*no sound*");
		    else 
		    {
		      MessageBox(28+46+34,13,(SoundMode==3?"sound on":"sound off"));
		      MessageBox(28+46+34,26,(SoundMode==0?"Low filter":(SoundMode==1?"High filter":"No filter")));
		      MessageBox(28+46+34,39,(SoundStereo?"mono":"stereo"));
		      MessageBox(28+46+34,52,(Sound16bits?"8bits":"16bits"));
		      if (snd_frequency==22050)
			MessageBox(28+46+34,64,"11KHz : 44KHz");
		      else
			if (snd_frequency==44100)
			  MessageBox(28+46+34,64,"11KHz : 22KHz");
			else
			  MessageBox(28+46+34,64,"22KHz : 44KHz");
		    }
		    SDL_UpdateRect(screen,0,0,0,0);			       
		    while (!SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);


		    if (enablesound)
		      {
			if ((y>13)&&(x>28+46+34))
			  {
			    if ((y<25)&&(x<28+46+34+54))
			      {
				if (SoundMode!=3) SoundMode=3;
				else SoundMode=InterPolateSnd;
			      }
			    else
			      if ((y<37)&&(x<28+46+34+60))
				{
				  InterPolateSnd++;
				  InterPolateSnd%=3;
				  if (SoundMode!=3) SoundMode=InterPolateSnd;
				}
			      else
				if ((y<49)&&(x<28+46+34+30))
				  {
				    SDL_CloseAudio();

				    SoundStereo^=1;
				    audio_buffer_pos=0;
				    buffposn=0;
				    buffer_filled=0;

				    if (nesemu) InitSound();
				    else
				      S9xReinitsound();
				    //				    Message("Not yet!\nUse command line\n",1);
				    //				    NewMSG=120;
				  }
				else
				  if ((y<61)&&(x<28+46+34+30))
				    {
				      SDL_CloseAudio();
				      Sound16bits^=1;
				      audio_buffer_pos=0;
				      buffposn=0;
				      buffer_filled=0;

				      if (nesemu) InitSound();
				      else
					S9xReinitsound();
				    }
				  else
				    if ((y<73)&&(x<28+46+34+78))
				      {
					SDL_CloseAudio();
					if (x<28+46+34+39)
					  {
					    if (snd_frequency==22050) snd_frequency=11025;
					    else 
					      if (snd_frequency==44100) snd_frequency=11025;
					      else snd_frequency=22050;
					  }
					else
					  {
					    if (snd_frequency==22050) snd_frequency=44100;
					    else 
					      if (snd_frequency==44100) snd_frequency=22050;
					      else snd_frequency=44100;
					  }
					audio_buffer_pos=0;
					buffposn=0;
					buffer_filled=0;
					
					if (nesemu) InitSound();
					else S9xReinitsound();
				    }
			  }
		      }
		  }
		else
		  if (x<28+46+34+34+52)
		    { //Controls
		      while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
		      //		      while (!SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
		      Message("Not yet!\nUse command line\n",1);
		      Message("Not yet!\nUse command line\n",0);
		      NewMSG=120;
		    }
		  else
		    if (x>255-33)
		      { //Help
			while (SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
			
			MessageBox(15,80,"\n  Little John New Generation 0.1.1  \n  (c) 2000 Yoyo  \n  Press F1 for inline help  \n  Click mouse to continue...  \n");

			SDL_UpdateRect(screen,0,0,0,0);
			
			while (!SDL_GetMouseState(&x,&y)) SDL_PollEvent(&event);
		      }

	  if ((SoundMode!=3)&&(enablesound)) 
	    {
	      SDL_PauseAudio(0);
	      audio_buffer_pos=0;
	      buffposn=0;
	      buffer_filled=0;
	    }
	}	

    }
  else 
    {
      if (viewGUI) memset((byte *)(screen->pixels),(bppix?0:13),screen->w*16<<bppix);
     
      viewGUI=0;
    }
  
  return(1); //continue emu
}

void InitInput();

void MainGUI(int argc,char **argv)
{
  char chaine[256*2];
  printf("Initalizing SDL...");
  if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|(enablesound?SDL_INIT_AUDIO:0)|SDL_INIT_JOYSTICK) < 0 ) 
  {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);

  InitInput();

  printf("ok\n");

   if (screen = SDL_SetVideoMode(256<<doubleres, 240<<doubleres, 16, SDL_SWSURFACE|(fullscreen?SDL_FULLSCREEN:0)))
     {
       bppix=1;
       pixsize=doubleres;
     }
   else
     {
       printf("Cannot open screen\n");
       exit(1);
     }   

   InitGUI();

   SDL_UpdateRect(screen,0,0,0,0);
   SDL_ShowCursor(1);
   SDL_WarpMouse(128,128);

   if (argc>1)
     {
       strcpy(CurrentROMFile,argv[1]);
       if (CurrentROMFile[0]=='-') CurrentROMFile[0]=0;
     }
   else
     CurrentROMFile[0]=0;

   quit=0;

   while (!quit)
    {
      if (CurrentROMFile[0]==0)
	{
	  quit=Reload_rom();
	}

      if (!quit)
	{
	  SDL_FreeSurface(screen);

	  nesemu=1;

	  keys=SDL_GetKeyState(NULL);

	  filechoosed=0;
	  if (nes_main(argc,argv))
	    {
	      nesemu=0;
	      snes_main(argc,argv);
	    }
	  if (!filechoosed) CurrentROMFile[0]=0;

	  if (keys[SDLK_ESCAPE]==SDL_PRESSED) quit=1;
	}
    }
}

void fillkeys(Uint8 *k)
{
  keys=SDL_GetKeyState(NULL);
  memcpy(k,keys,512);
}

void InitInput()
{
  int i,j=0;
  if (UseJoystickPl1) j++;
  if (UseJoystickPl2) j++;
  i=SDL_NumJoysticks();
  if (i<j) 
    {
      printf("** Not enough joysticks detected! Got : %d\n",i);
      if (UseJoystickPl2) { UseJoystickPl2=0; j--;};
      if (i<j) UseJoystickPl1=0;
    }
  if (UseJoystickPl1)
    {
      JoystickPl1Index=0;
      if ((joy1=SDL_JoystickOpen(JoystickPl1Index))==NULL) 
	{
	  printf("** Cannot open joystick!\n");
	  UseJoystickPl1=0;
	}
      if ((joy1nb=SDL_JoystickNumButtons(joy1))<2)
	{
	  printf("** 2 buttons joystick are the minimum!\n");
	  UseJoystickPl1=0;
	}
    }
  if (UseJoystickPl2)
    {
      if (UseJoystickPl1) JoystickPl2Index=1;
      else JoystickPl2Index=0;
      if ((joy2=SDL_JoystickOpen(JoystickPl2Index))==NULL) 
	{
	  printf("** Cannot open joystick!\n");
	  UseJoystickPl2=0;			
	}
      
     if ((joy2nb=SDL_JoystickNumButtons(joy2))<2)
	{
	  printf("** 2 buttons joystick are the minimum!\n");
	  UseJoystickPl2=0;
	}
    }
}

void LoadConfig()
{
  FILE *f;
  int i;
  f=fopen("ljohn.cfg","rt");
  if (f)
    {
      //      fscanf(f,"Roms path : %s\n",roms_dir);
      fscanf(f,"Roms path : ");
      fgets(roms_dir,256,f);
      roms_dir[strlen(roms_dir)-1]=0;
      fscanf(f,"Sound enable : %d\n",&enablesound);
      fscanf(f,"Sound frequency : %d\n",&snd_frequency);
      fscanf(f,"Sound stereo : %d\n",&SoundStereo);
      fscanf(f,"Sound 16bits : %d\n",&Sound16bits);
      fscanf(f,"Sound stereo : %d\n",&SoundStereo);
      fscanf(f,"Sound filter : %d\n",&InterPolateSnd);
      fscanf(f,"Buffer size : %d\n",&buffsize);
      fscanf(f,"Fullscreen : %d\n",&fullscreen);
      fscanf(f,"Double resolution : %d\n",&doubleres);
      fscanf(f,"Frameskip rate (255=AUTO): %d\n",&fskiprate);
      fscanf(f,"CPU cycle/scanline : %d\n",&cpucycle);
      fscanf(f,"Use joystick for player#1 : %d\n",&UseJoystickPl1);
      fscanf(f,"Use joystick for player#2 : %d\n",&UseJoystickPl2);
      fclose(f);
    }
}

void SaveConfig()
{
  FILE *f;

  f=fopen("ljohn.cfg","wt");
  if (f)
    {
      fprintf(f,"Roms path : %s\n",roms_dir);
      fprintf(f,"Sound enable : %d\n",enablesound);
      fprintf(f,"Sound frequency : %d\n",snd_frequency);
      fprintf(f,"Sound stereo : %d\n",SoundStereo);
      fprintf(f,"Sound 16bits : %d\n",Sound16bits);
      fprintf(f,"Sound stereo : %d\n",SoundStereo);
      fprintf(f,"Sound filter : %d\n",InterPolateSnd);
      fprintf(f,"Buffer size : %d\n",buffsize);
      fprintf(f,"Fullscreen : %d\n",fullscreen);
      fprintf(f,"Double resolution : %d\n",doubleres);
      fprintf(f,"Frameskip rate (255=AUTO): %d\n",fskiprate);
      fprintf(f,"CPU cycle/scanline : %d\n",cpucycle);
      fprintf(f,"Use joystick for player#1 : %d\n",UseJoystickPl1);
      fprintf(f,"Use joystick for player#2 : %d\n",UseJoystickPl2);
      fclose(f);
    } 

}

void InitVar()
{
  doubleres=0;
  fullscreen=0;

  ShowFR=FR=FRcnt=frame=0;

  usezapper=0;
  MasterVolume=255;

  SoundStereo=0;
  Sound16bits=0;
  buffsize=1024;
  snd_frequency=22050;
  SoundMode=0;

  enablesound=1;
  badsoundbuff=0,

  UseJoystickPl1=UseJoystickPl2=0;

  memset(ljng_keysym,0,256);
  ljng_keysym[A_1]=SDLK_c;
  ljng_keysym[B_1]=SDLK_x;
  ljng_keysym[X_1]=SDLK_s;
  ljng_keysym[Y_1]=SDLK_d;
  ljng_keysym[L_1]=SDLK_z;
  ljng_keysym[R_1]=SDLK_e;
  ljng_keysym[START_1]=SDLK_RETURN;
  ljng_keysym[SELECT_1]=SDLK_SPACE;
  ljng_keysym[LEFT_1]=SDLK_LEFT;
  ljng_keysym[RIGHT_1]=SDLK_RIGHT;
  ljng_keysym[UP_1]=SDLK_UP;
  ljng_keysym[DOWN_1]=SDLK_DOWN;

  ljng_keysym[A_2]=SDLK_END;
  ljng_keysym[B_2]=SDLK_DELETE;
  ljng_keysym[X_2]=SDLK_INSERT;
  ljng_keysym[Y_2]=SDLK_HOME;
  ljng_keysym[L_2]=SDLK_PAGEUP;
  ljng_keysym[R_2]=SDLK_PAGEDOWN;
  ljng_keysym[START_2]=SDLK_KP_ENTER;
  ljng_keysym[SELECT_2]=SDLK_KP_PLUS;
  ljng_keysym[LEFT_2]=SDLK_KP4;
  ljng_keysym[RIGHT_2]=SDLK_KP6;
  ljng_keysym[UP_2]=SDLK_KP8;
  ljng_keysym[DOWN_2]=SDLK_KP5;

  ljng_keysym[QUIT]=SDLK_ESCAPE;
  ljng_keysym[ACCEL]=SDLK_TAB;
  ljng_keysym[HELP]=SDLK_F1;

  ljng_keysym[SHIFTL]=SDLK_LSHIFT;
  ljng_keysym[SHIFTR]=SDLK_RSHIFT;
  ljng_keysym[CTRLL]=SDLK_LCTRL;
  ljng_keysym[CTRLR]=SDLK_RCTRL;

  strcpy(roms_dir,path_roms);

  filechoosed=0;
}

void helpsyntax()
{
  printf("  This version was built the 9th Mai 2000, at 06:26 in Caen(France)\n\n");
  printf("  *- CPU Core : Neil Corlett&Yohann Magnien(minor revisions)\n");
  printf("  *- Nes emulation : Yohann Magnien&Matthew Conte(sound documentation & code inspiration)\n");
  printf("     Be sure to check his great Nosefart .nsf player (nes music)\n");
  printf("  *- GUI : Yohann Magnien\n\n");
  printf("  *- Snes emulation : Snes9x\n");
  printf(" +- Have a look at README.TXT for more informations\n ^=^\n\n");

  helper();
  
  S9xUsage();
}



void LJNGParseArgs(int argc,char **argv)
{
  int i;
  if (argc>1)
    {
      i=1;
      while (i<argc)
	{
	  if ((strcmp("-h",argv[i])==0)||(strcmp("-?",argv[i])==0)||(strcmp("--help",argv[i])==0))
	    {
	      helpsyntax();
	      exit(0);
	    }
	  if (strcmp("-full",argv[i])==0) fullscreen=1;
	  if (strcmp("-win",argv[i])==0) fullscreen=0;

	   if (strcmp("-dres",argv[i])==0) doubleres=1;

	  if (strcmp("-nres",argv[i])==0) doubleres=0;
			
	  //has to be the last coz of i++
	  if (strcmp("-fskip",argv[i])==0)
	    {
	      i++;
	      if (i==argc) helper();
	      fskiprate=atoi(argv[i]);
				
	    }
			
	  if (strcmp("-nosound",argv[i])==0) enablesound=0;

	  if (strcmp("-sound",argv[i])==0) enablesound=1;
	  if (strcmp("-freq",argv[i])==0)
	    {
	      i++;
	      if (i==argc) helper();
	      snd_frequency=atoi(argv[i]);
	

	    }
	  if (strcmp("-buff",argv[i])==0)
	    {
	      i++;
	      if (i==argc) helper();
	      buffsize=atoi(argv[i]);
				
	    }

	  if (strcmp("-8",argv[i])==0) Sound16bits=0;
	  if (strcmp("-16",argv[i])==0) Sound16bits=1;
	  
	  if (strcmp("-mono",argv[i])==0) SoundStereo=0;
	  if (strcmp("-stereo",argv[i])==0) SoundStereo=1;

	  if (strcmp("-joy1",argv[i])==0) UseJoystickPl1=1;
	  if (strcmp("-joy2",argv[i])==0) UseJoystickPl2=1;

	  i++;
	}
    }
}

int main(int argc,char **argv)
{
#ifdef __CYGWIN32__
  SDL_RegisterApp("Little John", 0, GetModuleHandle(NULL));
#endif

  printf("\n [Little John New Generation v0.1.1], (c)2000 Yoyo\n");
  printf(" -----------------------------------------------\n\n");

  InitVar();
  LoadConfig();

  printf("dd %s\n",roms_dir);

  LJNGParseArgs(argc,argv);

  MainGUI(argc,argv);

  SaveConfig();

  return(0);
}

