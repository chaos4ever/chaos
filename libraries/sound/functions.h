/* Abstract: Function definitions for the sound library. */
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

#ifndef __LIBRARY_SOUND_FUNCTIONS_H__
#define __LIBRARY_SOUND_FUNCTIONS_H__

#include <ipc/ipc.h>
#include <system/system.h>
#include "types.h"

extern return_type sound_init(ipc_structure_type *ipc_structure, tag_type *tag);
/*extern return_type sound_configure_playmode (ipc_structure_type *ipc_structure,
                             sound_configuration_type *sound_configuration);*/
extern return_type sound_play_sample(ipc_structure_type *ipc_structure,
                                     sound_message_type *sound_message);
extern return_type sound_play_stream(ipc_structure_type *ipc_structure,
                                     sound_message_type *sound_message);


#endif /* !__LIBRARY_SOUND_FUNCTIONS_H__ */
