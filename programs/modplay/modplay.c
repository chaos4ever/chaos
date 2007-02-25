/* Abstract: Module player. */
/* Author: Erik Moren <nemo@chaosdev.org> */

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
#include "modfile.h"

#define BUFFER_SIZE 2000
#define FREQUENCY 22000
#define CHANNELS 4
#define ROWS 64
#define BYTES_PER_PATTERN ROWS*CHANNELS*4
//#define PAL 3546894
#define PAL 0x361F0E00     /* 3546894 << 2 */

ipc_structure_type ipc_structure;
log_structure_type log_structure;
console_structure_type console_structure;

tag_type empty_tag =
{
  0, 0, ""
};

/* Erik: Please put these in a header file, will you? /Hal */

typedef struct
{
  u8 name[20];
  u8 song_length;
  u8 play_sequence[128];
  u8 id[4];
  unsigned int physical_patterns;
} module_type;

module_type module;

typedef struct
{
  u8 name[22];
  u16 length;
  u8 finetune;
  u8 volume;
  u16 repeat_point;
  u16 repeat_length;
  u8 *data;
} sample_type;

typedef struct
{
  unsigned int sample_number;
  u16 length;
  u8 finetune;
  u8 volume;
  u16 repeat_point;
  u16 repeat_length;
  u32 scaling_factor;
  u8 *sample_data;
  u16 period_frequency;
  u32 ticks;
} channel_type;

typedef struct
{
  u8 sample_number : 8;
  u16 period_frequency : 12;
  u8 effect_number : 4;
  u8 effect_parameter : 8;
} __attribute__ ((packed)) note_type;

channel_type channel[CHANNELS];

sample_type sample[31];
u8 *pattern_data;
u8 *pattern[256];

unsigned int bpm;
unsigned int speed;
unsigned int mix_length;
unsigned int pattern_ticks;
unsigned int current_pattern;
unsigned int tick_length;
int current_row;

void fill_buffer (u8 *buffer);
void load_module (void);
void do_note (note_type *work_note, channel_type *work_channel);

int main (void)
{
//  unsigned int i;
//  note_type *note;
  sound_message_type *sound_message;

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Playing module...");

  console_init (&console_structure, &empty_tag, 
                IPC_CONSOLE_CONNECTION_CLASS_CLIENT);
  console_open (&console_structure, 80, 50, 4, VIDEO_MODE_TYPE_TEXT);
  console_use_keyboard (&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);
  console_clear (&console_structure);
  console_print (&console_structure,
                 "Module player\n");

  if (sound_init (&ipc_structure, &empty_tag) != SOUND_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not establish connection to a sound service.");
    return -1;
  }

  log_print (&log_structure, LOG_URGENCY_EMERGENCY,
             "Found sound service and established connection.");

  load_module ();
  memory_allocate ((void **) &sound_message,
                   sizeof (sound_message_type) + BUFFER_SIZE);
  sound_message->length = BUFFER_SIZE;
  sound_message->frequency = FREQUENCY;
  sound_message->bits = 8;

  /* Just some init data to get things going. */

  current_pattern = 0;
  current_row = 0;
  bpm = 125;
  speed = 6;
  tick_length = speed * ((FREQUENCY * 5) / bpm) >> 1;
  pattern_ticks = tick_length + 1;

  channel[0].volume = 63;
  channel[1].volume = 63;
  channel[2].volume = 63;
  channel[3].volume = 63;

#if FALSE
  note = (note_type *) pattern_data;
  for (i = 0; i < 10; i++)
  {
    console_print_formatted (&console_structure, "\n%x  %x",
                             note->sample_number, note->period_frequency);
    note++;
    console_print_formatted (&console_structure, " | %x  %x",
                             note->sample_number, note->period_frequency);
    note++;
    console_print_formatted (&console_structure, " | %x  %x",
                             note->sample_number, note->period_frequency);
    note++;
    console_print_formatted (&console_structure, " | %x  %x",
                             note->sample_number, note->period_frequency);
    note++;
  }
#endif

  system_call_process_parent_unblock ();

  while (TRUE)
  {
    fill_buffer (sound_message->data);

    if (sound_play_stream (&ipc_structure, sound_message) !=
        SOUND_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                 "Could not play the sample as wanted.");
      return -1;
    }

    console_cursor_move (&console_structure, 0, 5);
    console_print_formatted (&console_structure,
                             "Current Pattern: %x  "
                             "\nCurrent Row: %x  ",
                             current_pattern, current_row);
    console_print_formatted (&console_structure,
                             "Speed: %x  "
                             "\nBPM: %x  ",
                             speed, bpm);

/*    console_print_formatted (&console_structure, "\n%x %x %x   %x",
      current_row, channel[0].sample_number,
      channel[0].period_frequency,
      channel[0].scaling_factor); */
  }

  return 0;
}

void load_module (void)
{
  unsigned int i, j;
  unsigned int current_position;
  u8 temp_char1;
  u8 temp_char2;
  note_type *note;
  u8 temp_note[4];
  u8 *modfile_ptr;

#if FALSE
  /* Check to see if the module is of type "M.K." -> 31 channels. */
  if (string_compare_max("M.K.", &modfile[1080], 4) == 0)
  {
    //console_print(&console_structure, "\nModfile is of type M.K.");
  }
#endif

  /* Fetch module name. */

  memory_copy (module.name, &modfile[0], 20);
  module.name[19] = '\0';

  /* Fetch the sample data. */

  for (i = 0; i < 31; i++)
  {
    memory_copy (sample[i].name, &modfile[20 + i * 30], 22);
    sample[i].name[21] = '\0';
    temp_char1 = modfile[20 + i * 30 + 22];
    temp_char2 = modfile[20 + i * 30 + 23];
    sample[i].length = ((u16) temp_char1 * 256 + temp_char2) * 2;
    sample[i].finetune = modfile[20 + i * 30 + 24];
    sample[i].volume = modfile[20 + i * 30 + 25];
    temp_char1 = modfile[20 + i * 30 + 26];
    temp_char2 = modfile[20 + i * 30 + 27];
    sample[i].repeat_point = ((u16) temp_char1 * 256 + temp_char2) * 2;
    temp_char1 = modfile[20 + i * 30 + 28];
    temp_char2 = modfile[20 + i * 30 + 29];
    sample[i].repeat_length = ((u16) temp_char1 * 256 + temp_char2) * 2;
  }

  /* Load the song length (number of patterns to play), pattern
     sequence and the 4 id bytes (eg. 'M.K.'). */

  module.song_length = modfile[950];
  memory_copy (module.play_sequence, &modfile[952], 128);
  memory_copy (module.id, &modfile[1080], 4);

/*
  console_print(&console_structure,"\nplay_sequence: ");
  for (i = 0; i < 128; i++)
  {
  console_print_formatted(&console_structure,"%x: %x | ",
                            i, module.play_sequence[i]);
  }
*/

  /* Check to see how many different patterns we got. */

  module.physical_patterns = 0;

  for (i = 0; i < 128; i++)
  {
    if (module.play_sequence[i] > module.physical_patterns)
    {
      module.physical_patterns = module.play_sequence[i];
    }
  }
  module.physical_patterns++;
  console_print_formatted (&console_structure, "\nPhysical patterns: %x",
                           module.physical_patterns);

  /* Allocate memory for the pattern data and fill it. */

  memory_allocate ((void **) &pattern_data,
                   module.physical_patterns * BYTES_PER_PATTERN);

  /* We have to rearrange the order of the notedata because it is
     originally arranged in a really weird way. */

  note = (note_type *) pattern_data;
  modfile_ptr = (u8 *) &modfile[1084];

  for (i = 0; i < module.physical_patterns; i++)
  {
    for (j = 0; j < ROWS * CHANNELS; j++)
    {
      memory_copy (temp_note, modfile_ptr, 4);
      note->sample_number = (temp_note[0] & 0xF0) | ((temp_note[2] & 0xF0) >> 4);
      note->period_frequency = (u16) ((temp_note[0] & 0x0F) << 8) | temp_note[1];
      note->effect_number = temp_note[2] & 0x0F;
      note->effect_parameter = temp_note[3];

      modfile_ptr += 4;
      note++;
    }

    pattern[i] = &pattern_data[i * BYTES_PER_PATTERN];
  }

  /* Allocate memory for the samples and fill them. */

  current_position = 1084 + module.physical_patterns * BYTES_PER_PATTERN;

  for (i = 0; i < 31; i++)
  {
    /* If sample length is 0, then it's no idea to allocate and store it. */

    if (sample[i].length != 0)
    {
      memory_allocate ((void **) &sample[i].data, sample[i].length);
      for (j = 0; j < sample[i].length; j++)
      {
        sample[i].data[j] = modfile[current_position + j] ^ 0x80; /* XOR 80h */
      }
    }
    current_position += sample[i].length;
  }
}

void fill_buffer (u8 *buffer)
{
  unsigned int i, j;
  u32 k;
  note_type *note;
  u16 data;

  for (i = 0; i < BUFFER_SIZE; i++)
  {
    if (pattern_ticks++ > tick_length)
    {
      note = (note_type *) pattern[module.play_sequence[current_pattern]];
      note += (current_row * CHANNELS);

      current_row++;

      do_note (note + 0, channel + 0);
      do_note (note + 1, channel + 1);
      do_note (note + 2, channel + 2);
      do_note (note + 3, channel + 3);

      if (current_row == ROWS)
      {
        current_row = 0;
        current_pattern++;

        if (current_pattern == module.song_length)
        {
          current_pattern = 0;
        }
      }

      pattern_ticks = 0;
    }

    data = 0;

    for (j = 0; j < CHANNELS; j++)
    {
      channel[j].ticks += channel[j].scaling_factor;
      k = (channel[j].ticks >> 16);

      if (k >= channel[j].length)
      {
        channel[j].sample_data += channel[j].repeat_point;
        channel[j].length = channel[j].repeat_length;
        channel[j].repeat_point = 0;
        channel[j].ticks = 0;
        data += sample[channel[j].sample_number].data[0] * channel[j].volume;
      }
      else
      {
        data += sample[channel[j].sample_number].data[k] * channel[j].volume;
      }
    }

    buffer[i] = data >> 8;
  }
}

void do_note (note_type *work_note, channel_type *work_channel)
{
  if (work_note->period_frequency != 0)
  {
    if (work_note->sample_number != 0)
    {
      work_channel->sample_number = work_note->sample_number - 1;
    }

    work_channel->sample_data = sample[work_channel->sample_number].data;
    work_channel->length = sample[work_channel->sample_number].length;
    work_channel->repeat_point =
      sample[work_channel->sample_number].repeat_point;
    work_channel->repeat_length =
      sample[work_channel->sample_number].repeat_length;
    work_channel->period_frequency =
      work_note->period_frequency;
    work_channel->ticks = 0;

    work_channel->scaling_factor =
      (PAL / (u32) (work_channel->period_frequency)) << 8;
    work_channel->scaling_factor /= FREQUENCY;
  }

  switch (work_note->effect_number)
  {
    /* Set volume. */

    case 0xC:
    {
      console_cursor_move (&console_structure, 0, 24);
      console_print_formatted (&console_structure, "Effect C: Parameter = %x  ",
                               work_note->effect_parameter);
      
      work_channel->volume = work_note->effect_parameter;
      if (work_channel->volume > 0x40)
      {
        work_channel->volume = 0x40;
      }

      break;
    }

    /* Next pattern. */

    case 0xD:
    {
      current_pattern++;
      current_row = 0;

      /* FIXME: Should jump to right note also */

      break;
    }

    /* Set speed/BPM. */

    case 0xF:
    {
      console_cursor_move (&console_structure, 0, 25);
      console_print_formatted (&console_structure, "Effect F: Parameter = %x  ",
                               work_note->effect_parameter);

      if (work_note->effect_parameter < 0x20)
      {
        /* Speed. */

        speed = work_note->effect_parameter;
      }
      else
      {
        /* BPM. */

        bpm = work_note->effect_parameter;
      }

      tick_length = speed * ((FREQUENCY * 5) / bpm) >> 1;

      break;
    }
  }
}


