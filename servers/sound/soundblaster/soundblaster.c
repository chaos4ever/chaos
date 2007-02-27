/* $Id$ */
/* Abstract: Sound Blaster server */

/* Authors:
   
   Per Lundberg <plundis@chaosdev.org> 
   Erik Moren   <nemo@chaosdev.org> */

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

/* Most of the information in this server is based on information
   found in sblaster.doc, which can be found in the same directory as
   this source code. If it seems to be missing, please contact us:
   http://www.chaosdev.org/contact.shtml */

#include <log/log.h>
#include <system/system.h>
#include <storm/storm.h>
#include <sound/sound.h>

#include <soundblaster.h>
#include "config.h"

/* The default base port of the sound card. Can be overridden by a
   command parameter to the server (well, in the future...) */

u8 *dma_buffer;

double_buffer_type double_buffer[2];
unsigned int current_buffer = 0;

/* Change these to fit your needs. */

u16 base_port = 0x220;
unsigned int irq = 5;
unsigned int dma_channel = 1;

/* Version of the card. Valid versions are (from SBLASTER.DOC):

   SoundBlaster 1.0           1.??  (1.05???)
   SoundBlaster 1.5           1.??  (1.05???)
   SoundBlaster 2.0           2.xx  (2.01)
   SoundBlaster Pro           3.00  (???)
   SoundBlaster Pro 2         3.01+ (3.01, 3.02)
   SoundBlaster 16            4.0x  (4.04, 4.05)
   SoundBlaster 16 SCSI-2     4.11  (4.11)
   SoundBlaster AWE32         4.12+ (4.12) */

u8 major_version, minor_version;

log_structure_type log_structure;

/* FIXME: They should not be global variables, perhaps not even
   separate ones. */

soundblaster_device_type soundblaster_device;
soundblaster_event_type soundblaster_event;

static tag_type empty_tag =
{
  0, 0, ""
};

static void dsp_write (u8 data)
{
  while ((system_port_in_u8 (DSP_DATA_WRITE) & 0x80) != 0);
  system_port_out_u8 (DSP_DATA_WRITE, (data));
}

static u8 dsp_read (void)
{
  while ((system_port_in_u8 (DSP_DATA_AVAILABLE) & 0x80) == 0);
  return system_port_in_u8 (DSP_DATA_READ);
}

static u8 dsp_mixer_read (u8 which_register)
{
  system_port_out_u8 (DSP_MIXER_REGISTER, which_register);
  return system_port_in_u8 (DSP_MIXER_DATA);
}

static void dsp_mixer_write (u8 which_register, u8 data)
{
  system_port_out_u8 (DSP_MIXER_REGISTER, which_register);
  system_port_out_u8 (DSP_MIXER_DATA, data);
}

/* Detect if there is some kind of sound blaster card in this
   machine. */

static bool detect_sb (void)
{
  /* Register the I/O ports so that we can probe for the card. */

  system_call_port_range_register (base_port, 16, "Sound Blaster");

  /* Reset the DSP. */
  
  system_port_out_u8 (DSP_RESET, 0x01);
  system_sleep (4);
  system_port_out_u8 (DSP_RESET, 0x00);
  
  /* FIXME: Should only wait for a maximum of 100 us. */
  
  while ((system_port_in_u8 (DSP_DATA_AVAILABLE) & (1 << 7)) == 0);

  /* Check if the DSP was reset successfully. */

  if (system_port_in_u8 (DSP_DATA_READ) == 0xAA)
  {
    /* Let's check which kind of SB this is. */

    dsp_write (DSP_VERSION);
    major_version = dsp_read ();
    minor_version = dsp_read ();
    
    return TRUE;
  }
  else
  {
    system_call_port_range_unregister (base_port);
    return FALSE;
  }
}

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;

  /* Set our name. */

  system_call_process_name_set (PACKAGE_NAME);
  system_call_thread_name_set ("Initialising");
  
  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  /* Check for the presence of a Sound Blaster compatible card. */

  if (!detect_sb ())
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "No Sound Blaster compatible card detected.");
    return -1;
  }
  else
  {
    switch (major_version)
    {
      case 1:
      {
        log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                   "Sound Blaster 1.0/1.5 detected.");
        break;
      }
      case 2:
      {
        log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                   "Sound Blaster 2.0 detected.");
        break;
      }
      case 3:
      {
        log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                   "Sound Blaster Pro detected.");
        break;
      }
      case 4:
      {
        switch (minor_version)
        {
          case 1 ... 11:
          case 13:
          {
            log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                       "Sound Blaster 16 detected.");

            /* Set IRQ 5 and DMA 1. FIXME: Do this in a cleaner
             * way. */

            dsp_mixer_write (0x80, 2);
            dsp_mixer_write (0x81, 2);

            /* Set the volume. */

            dsp_mixer_write (0x22, 0xFF);
            break;
          }
          case 12:
          {
            log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                       "Sound Blaster AWE32 detected.");
            break;
          }
          default:
          {
            log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                       "Some kind of 16-bit Sound Blaster (compatible) detected.");
            break;
          }
        }
      }
    }
  }

  
  /* Because we only support soundblaster 2.0-functionality, treat every
     card as a sb2.0 whatever card is installed */

  soundblaster_device.irq = irq;
  soundblaster_device.base_port = base_port;
  soundblaster_device.dma_channel = dma_channel;
  soundblaster_device.max_frequency_output = 22050;
  soundblaster_device.supports_8bit_output = TRUE;
  soundblaster_device.supports_16bit_output = FALSE;
  soundblaster_device.supports_autoinit_dma = TRUE;
  soundblaster_device.device_name = "Sound Blaster 2.0";
 
  /* Register the DMA channel. */

  if (system_call_dma_register (soundblaster_device.dma_channel,
                                (void **) &dma_buffer) != STORM_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
               "!storm_return-sucess.afsd");
    return -1;
  }

  dsp_write (DSP_SPEAKER_ON);
  
  /* Create sound service. */
  
  if (ipc_service_create ("sound", &ipc_structure, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create service.");
    return -1;
  }
  
  system_thread_name_set ("Service handler");
  system_call_process_parent_unblock ();

  /* Create a new thread for the IPC stuff. */
  
  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;
    
    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;
    
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_thread_name_set ("Handling connection");
      
      handle_connection (reply_mailbox_id);
    }

    /* Install an interrupt handler. */
    
    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      irq_handler (reply_mailbox_id, ipc_structure,
                   soundblaster_device.irq);
    }
  }
}
 
void irq_handler (mailbox_id_type reply_mailbox_id,
                  ipc_structure_type ipc_structure, unsigned int irq_number)
{
  /* This is a really stupid way of doing things... */
  
  message_parameter_type message_parameter;
  
  ipc_structure.output_mailbox_id = reply_mailbox_id;
  system_call_thread_name_set ("IRQ handler");
  
  if (system_call_irq_register (irq_number, "Sound Blaster") !=
      STORM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY,
                         "Could not allocate IRQ %u.", irq_number);
    return;
  }

  while (TRUE)
  {
    unsigned char dummy_data[0];

    system_call_irq_wait (irq_number);

    /* Clear the SB interrupt. */
    
    system_port_in_u8 (DSP_DATA_AVAILABLE);
    
    /* Send a acknowledge message to the client program. */

    message_parameter.protocol = IPC_PROTOCOL_NONE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = 0;
    message_parameter.data = dummy_data;
    message_parameter.block = FALSE;
 
    if (ipc_send (ipc_structure.output_mailbox_id, &message_parameter) != 
        IPC_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_ERROR,
                 "ipc_send failed.");
      continue;
    }
 
    /* If only a single sample was being played, do some stuff. */
 
    if (!soundblaster_event.streaming)
    {
      soundblaster_event.is_playing = FALSE;
      dsp_write (DSP_SPEAKER_OFF);
    }

    system_call_irq_acknowledge (irq_number);
  }
}

/* Handle an IPC connection. */

void handle_connection (mailbox_id_type reply_mailbox_id)
{
  bool done = FALSE;
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  u8 *data;
 
  memory_allocate ((void **) &data, sizeof(sound_message_type) +
                   MAX_SINGLE_SAMPLE_SIZE);
  
  /* Accept the connection. */
  
  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);
 
  log_print (&log_structure, LOG_URGENCY_DEBUG,
             "soundblaster established connection.");
  
  /* Receive incoming requests. */
  
  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_NONE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = (sizeof (sound_message_type) +
                                MAX_SINGLE_SAMPLE_SIZE);
    message_parameter.data = data;
    message_parameter.block = TRUE;

    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     NULL) != IPC_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_ERROR,
                 "soundblaster mailbox_receive failed.");
      continue;
    }

    switch (message_parameter.message_class)
    {

      case IPC_SOUND_REGISTER:
      {
        /* FIXME: Do this. */

        break;
      }

      case IPC_SOUND_UNREGISTER:
      {
        /* FIXME: Do this. */

        break;
      }
      
      case IPC_SOUND_CONFIGURE_PLAYMODE:
      {
        /* FIXME: Do we really need this? hmm...OSS uses a similar */
      }

      case IPC_SOUND_PLAY_SAMPLE:
      {
        u16 time_constant;
        u16 length;
        sound_message_type *sound_message = (sound_message_type *) data;
        
        /* If parameters aren't checked, nobody wants to play the
           game. */
        /* FIXME: Check parameters here or in library. */
        
        /* Turn on speaker. It takes some time for the soundblaster
           to actually turn it on, so do it first just in case. */

         dsp_write (DSP_SPEAKER_ON);

         /* Copy sample data to dma buffer. */

         memory_copy (dma_buffer, sound_message->data,
                      sound_message->length);
 
 
         log_print (&log_structure, LOG_URGENCY_DEBUG, 
                    "Starting DMA transfer");

         /* Program DMA for transfer. */

         if (system_call_dma_transfer (soundblaster_device.dma_channel,
                                       sound_message->length,
                                       STORM_DMA_OPERATION_READ,
                                       STORM_DMA_TRANSFER_MODE_SINGLE,
                                       STORM_DMA_AUTOINIT_DISABLE) != TRUE)
         {
           log_print (&log_structure, LOG_URGENCY_INFORMATIVE,
                      "DMA transfer failed!!.");
         }

         /* Program Sound Blaster time constant. */

         time_constant = 65536 - (256000000 / sound_message->frequency);
         dsp_write (DSP_SET_TIME_CONSTANT);
         dsp_write ((u8)(time_constant >> 8));
         
         /* Program Sound Blaster buffer length (triggers an interrupt
            after 'length' bytes transferred) */

         length = sound_message->length-1;
         dsp_write (DSP_MODE_DMA_8BIT_DAC);
         dsp_write ((u8) length);
         dsp_write ((u8) (length >> 8));
 
         /* Sample is hopefully being played now...so set some
          * variables. */

         soundblaster_event.is_playing = TRUE;
         soundblaster_event.streaming = FALSE;
 
         break;
       }
 
       case IPC_SOUND_PLAY_STREAM:
       {
         u16 time_constant;
         u16 length;
         sound_message_type *sound_message = (sound_message_type *) data;

         /* If parameters aren't checked, nobody wants to play the
            game. */
         /* FIXME: Check parameters here or in library */
 
         double_buffer[0].data = &dma_buffer[0];
         double_buffer[1].data = &dma_buffer[sound_message->length];
 
         /* Fill double_buffer[current_buffer].data and then switch to
            the other double buffer. */
 
         memory_copy (double_buffer[current_buffer].data, sound_message->data,
                      sound_message->length);
         double_buffer[current_buffer].is_full = TRUE;

         /* Must be a better way to just switch 1->0 and vice
            versa. Nemo: Is this good enough? */

         current_buffer ^= 1;

         if (!soundblaster_event.is_playing)
         {
           /* If both buffers are full, and we have not yet started
              playing, it is time to do some hat tricks! */
 
           if (double_buffer[0].is_full && double_buffer[1].is_full)
           {
             
             /* Turn on speaker. It takes some time for the
                soundblaster to actually turn it on, so do it first
                just in case. */

             dsp_write (DSP_SPEAKER_ON);

             log_print (&log_structure, LOG_URGENCY_DEBUG, 
                        "Starting DMA transfer");
             
             /* Program DMA for transfer. */

             if (system_call_dma_transfer (soundblaster_device.dma_channel,
                                           sound_message->length * 2,
                                           STORM_DMA_OPERATION_READ,
                                           STORM_DMA_TRANSFER_MODE_SINGLE,
                                           STORM_DMA_AUTOINIT_ENABLE) != TRUE)
             {
               log_print (&log_structure, LOG_URGENCY_ERROR,
                          "DMA transfer failed.");
             }
 
             /* Program Sound Blaster time constant. */

             time_constant = 65536 - (256000000 / sound_message->frequency);
             dsp_write (DSP_SET_TIME_CONSTANT);
             dsp_write ((u8) (time_constant >> 8));
 
             /* Program Sound Blaster buffer length (triggers an
                interrupt after 'length' bytes transferred). */

             length = sound_message->length - 1;
             dsp_write (DSP_SET_DMA_BLOCK_SIZE);
             dsp_write ((u8) length);
             dsp_write ((u8) (length >> 8));
 
             /* FIXME: What is this? Use constants instead. */

             dsp_write (DSP_MODE_DMA_8BIT_AUTOINIT_DAC);
 
             /* Now the sample is being played hopefully, so set some
                variables. */

             soundblaster_event.is_playing = TRUE;
             soundblaster_event.streaming = TRUE;
           }
           else
           {
             /* Trigger the client to send more data because both
                double_buffers are not full. */

             unsigned char dummy_data[0];
 
             message_parameter.protocol = IPC_PROTOCOL_NONE;
             message_parameter.message_class = IPC_CLASS_NONE;
             message_parameter.length = 0;
             message_parameter.data = dummy_data;
             message_parameter.block = FALSE;
 
             if (ipc_send (ipc_structure.output_mailbox_id,
                           &message_parameter) != IPC_RETURN_SUCCESS)
             {
               log_print (&log_structure, LOG_URGENCY_ERROR,
                          "soundblaster mailbox_send failed.");
               continue;
             }
           }
         }
         break;
       }
 
       default:
       {
         log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                              "MESSAGE_CLASS: %x",
                              message_parameter.message_class);
         log_print (&log_structure, LOG_URGENCY_ERROR, "Unknown command.");
         break;
       }
    }
  }
}
