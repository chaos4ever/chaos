/* $Id$ */
/* Abstract: Video library. */
/* Authors: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#include <ipc/ipc.h>
#include <video/video.h>

/* Initialise the video library. */

return_type video_init (ipc_structure_type *video_structure, tag_type *tag)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Resolve the video services. */

  if (ipc_service_resolve ("video", mailbox_id, &services, 5, tag) !=
      IPC_RETURN_SUCCESS)
  {
    return VIDEO_RETURN_SERVICE_UNAVAILABLE;
  }

  /* Choose the appropriate service. We just use the first one for
     now. */

  video_structure->output_mailbox_id = mailbox_id[0];

  if (ipc_service_connection_request (video_structure) != IPC_RETURN_SUCCESS)
  {
    return VIDEO_RETURN_SERVICE_UNAVAILABLE;
  }
  
  return VIDEO_RETURN_SUCCESS;
}

/* FIXME */
/* Draw a line. */
/* Draw a circle. */
/* Copy a frame buffer to another. */

/* Set up a video mode. */

return_type video_mode_set 
  (ipc_structure_type *video_structure, video_mode_type *video_mode)
{
  message_parameter_type message_parameter;
  return_type return_value;

  /* Try to set the given mode. */

  message_parameter.protocol = IPC_PROTOCOL_VIDEO;
  message_parameter.message_class = IPC_VIDEO_MODE_SET;
  message_parameter.block = TRUE;
  message_parameter.length = sizeof (video_mode_type);
  message_parameter.data = video_mode;
  system_call_mailbox_send (video_structure->output_mailbox_id,
                            &message_parameter);

  /* Check if we got it. */

  message_parameter.length = sizeof (return_type);
  message_parameter.data = &return_value;
  system_call_mailbox_receive (video_structure->input_mailbox_id, 
                               &message_parameter);

  /* Handle errors. */

  if (return_value == IPC_VIDEO_RETURN_MODE_UNAVAILABLE)
  {
    return VIDEO_RETURN_MODE_UNAVAILABLE;
  }

  /* Return success. */
  
  return VIDEO_RETURN_SUCCESS;
}
