// Abstract: Module player.
// Author: Erik Moren <nemo@chaosdev.org>
//         Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include <console/console.h>
#include <ipc/ipc.h>
#include <log/log.h>
#include <memory/memory.h>
#include <sound/sound.h>
#include <string/string.h>
#include <system/system.h>

#include "modfile.h"
#include "modplay.h"
#include "hxcmod.h"

ipc_structure_type ipc_structure;
log_structure_type log_structure;
console_structure_type console_structure;

tag_type empty_tag =
{
    0, 0, ""
};

int main(void)
{
    sound_message_type *sound_message;
    const char *program_name = "modplay";

    log_init(&log_structure, program_name, &empty_tag);

    system_call_process_name_set(program_name);
    system_call_thread_name_set("Playing module");

    console_init(&console_structure, &empty_tag,
                 IPC_CONSOLE_CONNECTION_CLASS_CLIENT);

    ipc_console_attribute_type ipc_console_attribute = {
        mode_type: VIDEO_MODE_TYPE_TEXT,
        width: 80,
        height: 50,
        depth: 4,
        activate: TRUE
    };

    console_open(&console_structure, ipc_console_attribute);
    console_use_keyboard(&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);
    console_clear(&console_structure);
    console_print(&console_structure,
                  "Module player\n");

    if (sound_init(&ipc_structure, &empty_tag) != SOUND_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                  "Could not establish connection to a sound service.");
        return -1;
    }

    log_print(&log_structure, LOG_URGENCY_EMERGENCY,
              "Found sound service and established connection.");

    if (memory_allocate((void **) &sound_message, sizeof(sound_message_type) + BUFFER_SIZE) !=
                        MEMORY_RETURN_SUCCESS) {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                  "Memory allocation failed");
        return -1;
    }

    sound_message->length = BUFFER_SIZE;
    sound_message->frequency = FREQUENCY;
    sound_message->bits = 8;

    system_call_process_parent_unblock();

    modcontext modctx;

    if (hxcmod_init(&modctx) == 0)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "hxcmod initialization failed");
        return -1;
    }

    if (hxcmod_load(&modctx, &modfile[0], modfile_size) == 0)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "hxcmod module parsing failed");
        return -1;
    }

    console_print_formatted(&console_structure, "Playing %s\n", modctx.song.title);

    while (TRUE)
    {
        // From the hxcmod documentation (note that stereo 16bits is not entirely true, we override
        // it to be 8-bit mono for now)

        // void hxcmod_fillbuffer( modcontext * modctx, unsigned short * outbuffer, unsigned long nbsample, tracker_buffer_state * trkbuf )

        // - Generate and return the next samples chunk to outbuffer.
        //   nbsample specify the number of stereo 16bits samples you want.
        //   The output format is signed 44100Hz 16-bit Stereo PCM samples.
        //   The output buffer size in byte must be equal to ( nbsample * 2 * 2 ).
        //   The optional trkbuf parameter can be used to get detailed status of the player. Put NULL/0 is unused.

        hxcmod_fillbuffer(&modctx, sound_message->data, BUFFER_SIZE * NUM_CHANNELS * BYTES_PER_SAMPLE, NULL);

        if (sound_play_stream(&ipc_structure, sound_message) !=
                SOUND_RETURN_SUCCESS)
        {
            log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                      "Could not play the sample as wanted.");
            return -1;
        }
    }

    return 0;
}
