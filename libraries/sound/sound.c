/* Abstract: Sound library. */
/* Author: Erik Moren <nemo@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

/* See The chaos Programming Reference Manual for more information
   about the functions in this library. */

#include <ipc/ipc.h>
#include <memory/memory.h>
#include <sound/sound.h>
#include <string/string.h>
#include <system/system.h>


extern return_type sound_init(ipc_structure_type *ipc_structure, tag_type *tag)
{
  /* FIXME: This function doesn't register itself with the server, but
     that isn't even working in the server yet so don't worry */

  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Try to resolve the sound service. */

  if (ipc_service_resolve ("sound", mailbox_id, &services, 5, tag) !=
      IPC_RETURN_SUCCESS)
  {
    return SOUND_RETURN_SERVICE_UNAVAILABLE;
  }

  ipc_structure->output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (ipc_structure) != IPC_RETURN_SUCCESS)
  {
    return SOUND_RETURN_SERVICE_UNAVAILABLE;
  }

  return SOUND_RETURN_SUCCESS;


}
#if FALSE
return_type sound_configure_playmode (ipc_structure_type *ipc_structure,
                           sound_configuration_type *sound_configuration)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_SOUND;
  message_parameter.message_class = IPC_SOUND_CONFIGURE_PLAYMODE;
  message_parameter.data = (void *)sound_configuration;
  message_parameter.length = sizeof(sound_configuration_type);
  message_parameter.block = TRUE;

  ipc_send (ipc_structure->output_mailbox_id, &message_parameter);

}
#endif

return_type sound_play_sample(ipc_structure_type *ipc_structure,
                              sound_message_type *sound_message)
{
  message_parameter_type message_parameter;
  unsigned char dummy[0];

  message_parameter.protocol = IPC_PROTOCOL_SOUND;
  message_parameter.message_class = IPC_SOUND_PLAY_SAMPLE;
  message_parameter.data = sound_message;
  message_parameter.length = sizeof(sound_message_type) +
                             sound_message->length;
  message_parameter.block = FALSE;

  ipc_send (ipc_structure->output_mailbox_id, &message_parameter);

  /* Receive a message when the sample is played. */

  message_parameter.data = dummy;
  message_parameter.block = TRUE;
  message_parameter.length = 0;

  ipc_receive (ipc_structure->input_mailbox_id, &message_parameter, NULL);

  return SOUND_RETURN_SUCCESS;
}

return_type sound_play_stream (ipc_structure_type *ipc_structure,
                               sound_message_type *sound_message)
{
  message_parameter_type message_parameter;
  unsigned char dummy[0];

  message_parameter.protocol = IPC_PROTOCOL_SOUND;
  message_parameter.message_class = IPC_SOUND_PLAY_STREAM;
  message_parameter.data = sound_message;
  message_parameter.length = sizeof(sound_message_type) +
                             sound_message->length;
  message_parameter.block = FALSE;

  ipc_send (ipc_structure->output_mailbox_id, &message_parameter);

  /* Receive a message when the sample is played. */

  message_parameter.data = dummy;
  message_parameter.block = TRUE;
  message_parameter.length = 0;

  ipc_receive (ipc_structure->input_mailbox_id, &message_parameter, NULL);

  return SOUND_RETURN_SUCCESS;
}
