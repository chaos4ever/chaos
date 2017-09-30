// Abstract: Sound server IPC message types.
// Author: Erik Moren <nemo@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

// Message types.
enum
{
    IPC_SOUND_REGISTER,
    IPC_SOUND_UNREGISTER,
    IPC_SOUND_CONFIGURE_PLAYMODE,
    IPC_SOUND_PLAY_SAMPLE,
    IPC_SOUND_PLAY_STREAM,
    IPC_SOUND_STOP
};
