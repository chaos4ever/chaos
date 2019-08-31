/* $Id$ */
/* Abstract: tornado test. Very, very non-functional so far. :-) */
/* Authors: Per Lundberg <per@chaosdev.io>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include "config.h"
#include "font_8x8.h"
#include "mouse_cursor.h"

#define WIDTH           320
#define HEIGHT          200
#define DEPTH           8

#define TITLEBAR_HEIGHT 10

#define B00011100       0x1C
#define B11100000       0xE0

/* Change the palette if you want to change the colours. */

enum
{
  COLOUR_BLACK = 0x000000,
  COLOUR_NORMAL = 0x6060A0,
  COLOUR_BRIGHT = (COLOUR_NORMAL + 0x404040),
  COLOUR_DARK = (COLOUR_NORMAL - 0x404040),
  COLOUR_BACKGROUND = 0x00404C,
  COLOUR_WHITE = 0xFFFFFF,
  COLOUR_TEXT = COLOUR_WHITE,
  COLOUR_TITLEBAR = 0x545494,
};

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

/* 0 = transparent, 1 = black, 2 = dark, 3 = normal, 4 = bright, 6 = white */

static uint8_t old_mouse_cursor[16][16];

/* This is not the final solution to the question about Life, the
   Universe, and Everything. */

static uint8_t *screen = (uint8_t *) 0xA0000;

/* This function is used for converting a 32-bit colour value to a 332
   index. */

static log_structure_type log_structure;
static console_structure_type console_structure;

static inline uint8_t argb_to_332 (uint32_t argb)
{
  uint8_t r = argb >> 16;
  uint8_t g = argb >> 8;
  uint8_t b = argb >> 0;

  return (b >> 6) | ((g >> 3) & (B00011100)) | (r & B11100000);
}

/* Inlines for putting pixels. */

static inline void tornado_putpixel8 (uint8_t *buffer, uint32_t x, uint32_t y, uint32_t argb)
{
  uint8_t index = argb_to_332 (argb);

  buffer[y * WIDTH + x] = index;
}

static inline void tornado_putpixel32 (uint32_t *buffer, uint32_t x, uint32_t y, uint32_t argb)
{
  buffer[y * WIDTH + x] = argb;
}

static void tornado_draw_box (uint32_t *buffer, unsigned int x,
                              unsigned int y, unsigned int width,
                              unsigned int height, uint32_t colour)
{
  unsigned int x_loop, y_loop;

  if (DEPTH == 32)
  {
    for (y_loop = y; y_loop < y + height; y_loop++)
    {
      memory_set_uint32_t (&buffer[y_loop * WIDTH + x], colour, width);
    }
  }
  else if (DEPTH == 8)
  {
    for (y_loop = y; y_loop < y + height; y_loop++)
    {
      for (x_loop = x; x_loop < x + width; x_loop++)
      {
        tornado_putpixel8 ((uint8_t *) buffer, x_loop, y_loop, colour);
      }
    }
  }
}

static void tornado_fill_screen (uint32_t *buffer, uint32_t argb)
{
  if (DEPTH == 32)
  {
    memory_set_uint32_t (buffer, argb, WIDTH * HEIGHT);
  }
  else if (DEPTH == 8)
  {
    uint8_t index = argb_to_332 (argb);

    memory_set_uint8_t ((uint8_t *) buffer, index, WIDTH * HEIGHT);
  }
}

static void tornado_draw_hline (uint32_t *buffer, unsigned int x,
                                unsigned int y, unsigned int width,
                                uint32_t colour)
{
  if (DEPTH == 32)
  {
    memory_set_uint32_t (&buffer[y * WIDTH + x], colour, width);
  }
  else if (DEPTH == 8)
  {
    uint8_t index = argb_to_332 (colour);
    uint8_t *buffer_uint8_t = (uint8_t *) buffer;

    memory_set_uint8_t (&buffer_uint8_t[y * WIDTH + x], index, width);
  }
}

static void tornado_draw_vline (uint32_t *buffer, unsigned int x,
                                unsigned int y, unsigned int height,
                                uint32_t colour)
{
  unsigned int y_loop;

  if (DEPTH == 32)
  {
    for (y_loop = y; y_loop < y + height; y_loop++)
    {
      buffer[y_loop * WIDTH + x] = colour;
    }
  }
  else if (DEPTH == 8)
  {
    int index = argb_to_332 (colour);

    for (y_loop = y; y_loop < y + height; y_loop++)
    {
      ((uint8_t *) buffer)[y_loop * WIDTH + x] = index;
    }
  }
}

#if FALSE
static void tornado_draw_3dframe (uint32_t *buffer, unsigned int x,
                                  unsigned int y, unsigned int width,
                                  unsigned int height, int type)
{
  switch (type)
  {
    case 0:
    {
      /* Outer box. */

      tornado_draw_hline (buffer, x, y, width, COLOUR_NORMAL);
      tornado_draw_hline (buffer, x, y + height - 1, width, COLOUR_BLACK);
      tornado_draw_vline (buffer, x, y + 1, height - 2, COLOUR_NORMAL);
      tornado_draw_vline (buffer, x + width - 1, y + 1, height - 2,
                          COLOUR_BLACK);

      /* Inner circle. */

      tornado_draw_hline (buffer, x + 1, y + 1, width - 2, COLOUR_BRIGHT);
      tornado_draw_hline (buffer, x + 1, y + height - 2, width - 2,
                          COLOUR_DARK);
      tornado_draw_vline (buffer, x + 1, y + 2, height - 4, COLOUR_BRIGHT);
      tornado_draw_vline (buffer, x + width - 2, y + 2, height - 4,
                          COLOUR_DARK);

      break;
    }

    case 1:
    {
      break;
    }
  }
}
#endif

static void tornado_draw_3dframe_thin (uint32_t *buffer, unsigned int x,
                                       unsigned int y, unsigned int width,
                                       unsigned int height, int type)
{
  switch (type)
  {
    case 0:
    {
      tornado_draw_hline (buffer, x, y, width, COLOUR_BRIGHT);
      tornado_draw_hline (buffer, x, y + height - 1, width, COLOUR_DARK);
      tornado_draw_vline (buffer, x, y, height, COLOUR_BRIGHT);
      tornado_draw_vline (buffer, x + width - 1, y, height, COLOUR_DARK);

      break;
    }

    case 1:
    {
      tornado_draw_hline (buffer, x, y, width, COLOUR_DARK);
      tornado_draw_hline (buffer, x, y + height - 1, width, COLOUR_BRIGHT);
      tornado_draw_vline (buffer, x, y, height, COLOUR_DARK);
      tornado_draw_vline (buffer, x + width - 1, y, height, COLOUR_BRIGHT);

      break;
    }
  }
}

static void tornado_draw_frame (uint32_t *buffer, unsigned int x,
                                unsigned int y, unsigned int width,
                                unsigned int height, uint32_t colour)
{
  tornado_draw_hline (buffer, x, y, width, colour);
  tornado_draw_hline (buffer, x, y + height - 1, width, colour);
  tornado_draw_vline (buffer, x, y + 1, height - 2, colour);
  tornado_draw_vline (buffer, x + width - 1, y + 1, height - 2, colour);
}

static inline void tornado_flip (uint32_t *destination, uint32_t *source)
{
  memory_copy (destination, source, WIDTH * HEIGHT * (DEPTH / 8));
}

static void tornado_draw_text (uint32_t *buffer, unsigned int x,
                               unsigned int y, char *text, uint32_t colour)
{
  uint32_t y_loop;
  uint32_t x_loop;

  while (*text != '\0')
  {
    for (y_loop = 0; y_loop < 8; y_loop++)
    {
      for (x_loop = 0; x_loop < 8; x_loop++)
      {
	if (SYSTEM_BIT_GET (font_8x8[*text * 8 + y_loop], 7 - x_loop))
        {
          if (DEPTH == 32)
          {
            tornado_putpixel32 (buffer, x_loop + x, y_loop + y, colour);
          }
          else if (DEPTH == 8)
          {
            tornado_putpixel8 ((uint8_t *) buffer, x_loop + x, y_loop + y, colour);
          }
        }
      }
    }
    x += 8;
    text++;
  }
}

static void tornado_draw_window (uint32_t *buffer, unsigned int x,
                                 unsigned int y, unsigned int width,
                                 unsigned int height, char *name)
{
  tornado_draw_box (buffer, x + 2, y + 2, width - 4, height - 4,
                    COLOUR_NORMAL);
  tornado_draw_3dframe_thin (buffer, x + 1, y + 1, width - 2, height - 2, 0);
  tornado_draw_frame (buffer, x, y, width, height, COLOUR_BLACK);
  tornado_draw_3dframe_thin (buffer, x + 4, y + 4, width - 8,
                             TITLEBAR_HEIGHT + 2, 1);
  tornado_draw_box (buffer, x + 5, y + 5, width - 10, TITLEBAR_HEIGHT,
                    COLOUR_TITLEBAR);
  tornado_draw_text (buffer, x + 6, y + 6, name, COLOUR_TEXT);
}

static void tornado_draw_mouse_cursor (int x_position,
                                       int y_position,
                                       int old_x, int old_y)
{
  int x, y;

  /* Are we run for the first time? If so, start by backing up the
     screen. */

  if (old_x == -1 && old_y == -1)
  {
    for (y = 0; y < MOUSE_CURSOR_HEIGHT; y++)
    {
      memory_copy (&old_mouse_cursor[y],
                   &screen[(y_position + y) * WIDTH + x_position],
                   MOUSE_CURSOR_WIDTH * (DEPTH / 8));
    }
  }
  else
  {
    /* Restore the screen. */

    for (y = 0; y < MOUSE_CURSOR_HEIGHT; y++)
    {
      if (old_y + y < HEIGHT)
      {
	memory_copy (&screen[(old_y + y) * WIDTH + old_x],
                     &old_mouse_cursor[y], MOUSE_CURSOR_WIDTH * (DEPTH / 8));
      }
    }

    /* Save the new area. */

    for (y = 0; y < 16; y++)
    {
      if (y_position + y < HEIGHT)
      {
	memory_copy (&old_mouse_cursor[y],
                     &screen[(y_position + y) * WIDTH + x_position],
                     16 * (DEPTH / 8));
      }
    }
  }

  /* Draw the mouse cursor. */

  for (y = 0; y < 16; y++)
  {
    for (x = 0; x < 16; x++)
    {
      if (tornado_standard_mouse_cursor[y][x] && x_position + x < WIDTH &&
	  y_position + y < HEIGHT)
      {
        uint32_t c = 0;

        switch (tornado_standard_mouse_cursor[y][x])
        {
          case 1:
          {
            c = COLOUR_BLACK;
            break;
          }
          case 2:
          {
            c = COLOUR_DARK;
            break;
          }
          case 3:
          {
            c = COLOUR_NORMAL;
            break;
          }
          case 4:
          {
            c = COLOUR_BRIGHT;
            break;
          }
          case 6:
          {
            c = COLOUR_WHITE;
            break;
          }
        }

        if (DEPTH == 32)
        {
          tornado_putpixel32 ((uint32_t *) screen, x_position + x, y_position + y,
                              c);
        }
        else if (DEPTH == 8)
        {
          tornado_putpixel8 ((uint8_t *) screen, x_position + x, y_position + y, c);
        }
      }
    }
  }
}

int main (void)
{
  char tmpstr[] = "chaos 0.3.0 installation";
  int mouse_x = WIDTH / 2, mouse_y = HEIGHT / 2;

  /* Set our name. */

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Initialising");

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  if (console_init (&console_structure, &empty_tag) != CONSOLE_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not connect to console service.");
    return -1;
  }

  if (console_open (&console_structure, WIDTH, HEIGHT, DEPTH,
                    VIDEO_MODE_TYPE_GRAPHIC) != VIDEO_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not open a graphical console.");
    return -1;
  }

  console_use_mouse (&console_structure, TRUE);

  system_sleep (1000);

  tornado_fill_screen ((uint32_t *) screen, COLOUR_BACKGROUND);
  tornado_draw_text ((uint32_t *) screen, WIDTH - string_length (tmpstr) * 8 - 2,
                     HEIGHT - 10, tmpstr, COLOUR_TEXT);

  tornado_draw_window ((uint32_t *) screen, 5, 5, 300, 180,
                       tmpstr);

  tornado_draw_mouse_cursor (mouse_x, mouse_y, -1, -1);

  while (TRUE)
  {
    ipc_mouse_event_type ipc_mouse_event;
    int event_type;

    console_event_wait (&console_structure, &ipc_mouse_event,
                        &event_type, TRUE);

    switch (event_type)
    {
      case CONSOLE_EVENT_MOUSE:
      {
        tornado_draw_mouse_cursor (ipc_mouse_event.x, ipc_mouse_event.y,
                                   mouse_x, mouse_y);
        mouse_x = ipc_mouse_event.x;
        mouse_y = ipc_mouse_event.y;
      }
    }
  }
}
