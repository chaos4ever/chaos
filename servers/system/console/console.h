/* $Id$ */
/* Abstract: Console header file. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <ipc/ipc.h>

/* The size of all mailboxes created by the console server. */

#define CONSOLE_MAILBOX_SIZE            1024

/* The default attribute to use for newly created consoles. */

#define CONSOLE_DEFAULT_ATTRIBUTE       0x07

/* The location of the physical video memory. */

#define CONSOLE_VIDEO_MEMORY            0xB8000
#define CONSOLE_VIDEO_MEMORY_SIZE       (32 * KB)

/* Maximum number of numeric arguments per escape sequence. */

#define MAX_NUMBER_OF_NUMERIC_PARAMETERS 10

enum
{
  CONSOLE_KEYBOARD_NORMAL,
  CONSOLE_KEYBOARD_EXTENDED
};

typedef struct
{
  u8 character;
  u8 attribute;
} __attribute__ ((packed)) character_type;

/* A console application structure. */

typedef struct
{
  /* Is this application listening for keyboard events? And how
     verbose should we be? */

  bool wants_keyboard;
  int keyboard_type;

  /* And/Or mouse? */

  bool wants_mouse;

  /* So we know where to send our stuff. */

  ipc_structure_type ipc_structure;

  /* Next application on this console. */

  struct console_application_type *next;
} console_application_type;

/* A virtual console structure. */

typedef struct
{
  /* Location of the cursor. */

  int cursor_x;
  int cursor_y;

  /* Stored location of the cursor. */

  int cursor_saved_x;
  int cursor_saved_y;

  /* If type is VIDEO_MODE_TYPE_TEXT, characters. Otherwise,
     pixels. */

  int width;
  int height;  
  int depth;
  int type;

  /* The current state of the console. Used for ANSI escape
     sequences. */

  unsigned int state;
  unsigned int numeric_argument_index;
  int numeric_argument[MAX_NUMBER_OF_NUMERIC_PARAMETERS];

  /* FIXME: This should be extended to support other formats. Right
     now, we just use the EGA attribute. */

  u8 current_attribute;

  /* Pointer to the buffer for this console. */

  character_type *buffer;

  /* The actual output to this console should go here. When the
     console is activated, this and the previous one will differ. */

  character_type *output;

  /* FIXME: This isn't neccessary. */

  u8 modified_attribute;

  int character_width;
  int character_height;

  bool bold;
  bool underline;
  bool blink;
  bool inverse;

  /* The current application receives input events. (keyboard and
     mouse). */

  console_application_type *active_application;

  /* This linked list holds information about all the applications
     connected to this specific console. */

  console_application_type *application_list;

  /* Is this console locked? */
  /* FIXME: Real mutexing is needed. */
  
  bool lock;

  /* Next console. */

  struct console_type *next;
} console_type;

/* States for a console. */

enum
{
  CONSOLE_STATE_CHARACTER,
  CONSOLE_STATE_PREESCAPE,
  CONSOLE_STATE_ESCAPE,
  CONSOLE_STATE_NUMERIC,
};

/* Global variables. */

extern character_type *screen;
extern volatile bool has_video;
extern ipc_structure_type video_structure;
extern volatile console_type *current_console;
extern volatile unsigned int number_of_consoles;
extern console_type *console_list;
extern volatile console_type *console_shortcut[];

/* External functions. */

extern void handle_connection (mailbox_id_type reply_mailbox_id) __attribute__ ((noreturn));
extern void console_link (console_type *console);
extern void console_flip (console_type *console);

#endif /* !defined __CONSOLE_H__ */
