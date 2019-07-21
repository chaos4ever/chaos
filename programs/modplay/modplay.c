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

modcontext modctx;

#define NUM_MODFILES 5

uint8_t *modfiles[] = {
    modfile_axelf,
    modfile_breath,
    modfile_chipmunk,
    modfile_enigma,
    modfile_skogen10
};

int modfile_sizes[NUM_MODFILES];

static bool select_modfile(int current_mod_file);
static int initialize_mod_player(uint8_t *modfile, int length);
static void initialize_tracker_buffer_state(tracker_buffer_state *trackbuf_state);
static void print_mod_name(void);
static uint16_t abs(int16_t value);

int main(void)
{
    // Can't be statically initialized since these are not compile-time constants.
    modfile_sizes[0] = modfile_axelf_size;
    modfile_sizes[1] = modfile_breath_size;
    modfile_sizes[2] = modfile_chipmunk_size;
    modfile_sizes[3] = modfile_enigma_size;
    modfile_sizes[4] = modfile_skogen10_size;

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
        activate: TRUE,
        enable_buffer: TRUE
    };

    console_open(&console_structure, ipc_console_attribute);
    console_use_keyboard(&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);
    console_cursor_move(&console_structure, 80, 50);
    console_buffer_print_formatted(&console_structure, 0, 0, 0x1F, "%-80s",
                                   " [\xFA] chaos Module Player");

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

    // The index of the .mod file currently playing
    int current_mod_file = 0;
    int current_mod_file_selecting = 0;

    if (!select_modfile(current_mod_file))
    {
        return -1;
    }

    int event_type;
    keyboard_packet_type keyboard_packet;
    tracker_buffer_state trackbuf_state;

    initialize_tracker_buffer_state(&trackbuf_state);
    int visualizations[12];
    int channel_visualizations[NUMMAXCHANNELS];
    int active_instruments[NUM_INSTRUMENTS];

    memory_set_uint32_t((uint32_t *) &visualizations, 0, 12);
    memory_set_uint32_t((uint32_t *) &channel_visualizations, 0, NUMMAXCHANNELS);

    while (TRUE)
    {
        // From the hxcmod documentation (note that stereo 16bits is not entirely true, we override
        // it to be 8-bit mono for now). Likewise, nbsample * 2 *2 presumes 16-bit stereo is being
        // used.

        // void hxcmod_fillbuffer( modcontext * modctx, unsigned short * outbuffer, unsigned long nbsample, tracker_buffer_state * trkbuf )

        // - Generate and return the next samples chunk to outbuffer.
        //   nbsample specify the number of stereo 16bits samples you want.
        //   The output format is signed 44100Hz 16-bit Stereo PCM samples.
        //   The output buffer size in byte must be equal to ( nbsample * 2 * 2 ).
        //   The optional trkbuf parameter can be used to get detailed status of the player. Put NULL/0 is unused.

        trackbuf_state.nb_of_state = 0;
        hxcmod_fillbuffer(&modctx, sound_message->data, NUMBER_OF_SAMPLES, &trackbuf_state);

        if (sound_play_stream(&ipc_structure, sound_message) !=
                SOUND_RETURN_SUCCESS)
        {
            log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                      "Could not play the sample as wanted.");
            return -1;
        }

        // Print info about instruments/samples.
        memory_set_uint32_t(active_instruments, 0, NUM_INSTRUMENTS);

        for (int i = 0; i < modctx.number_of_channels; i++)
        {
            active_instruments[modctx.channels[i].sampnum] = i;
        }

        for (unsigned int i = 0; i < 31; i++)
        {
            int color = (active_instruments[i] > 0 ?
                BRIGHT_INSTRUMENT_COLOR :
                DEFAULT_INSTRUMENT_COLOR);

            console_buffer_print_formatted(&console_structure, 54, 5 + i,
                                           color, "%02d: %-22s",
                                           i, trackbuf_state.instruments[i].name);
        }

        // Attempt to do some form of channel visualization
        for (int i = 0; i < modctx.number_of_channels; i++)
        {
            track_state *track = &trackbuf_state.track_state_buf[0].tracks[i];
            int sample_position = modctx.channels[i].samppos >> 10;

            if (sample_position != 0 && modctx.channels[i].sampdata != NULL)
            {
                short pcm_data = modctx.channels[i].sampdata[sample_position]  * track->cur_volume;

                int previous = channel_visualizations[i];

                // Special-case so that 32768 becomes 32767, i.e. small enough to fit in 15 bits.
                channel_visualizations[i] = MIN_OF_TWO(abs(pcm_data), 32767);

                // Try to "slow down" the changes in the visualizations a bit, to make it look
                // less flickery. TODO: could try even more than 2 samples/channel to see if it
                // looks even better.
                channel_visualizations[i] = (channel_visualizations[i] + previous) / 2;
            }
            else
            {
                // Purpose of this: to avoid the bar going all the way down, just to go up again
                // very shortly after; this can look quite flickery.
                // int previous = channel_visualizations[i];
                // channel_visualizations[i] = 0;
                // channel_visualizations[i] = (channel_visualizations[i] + previous) / 2;

                // Purpose of this: to avoid the bar going all the way down, just to go up again
                // very shortly after; this can look quite flickery.
                channel_visualizations[i] -= 1;

                if (channel_visualizations[i] < 0)
                {
                    channel_visualizations[i] = 0;
                }
            }
        }

        // Print one bar for each part of the spectrum
        for (int i = 0; i < modctx.number_of_channels; i++)
        {
            char bar[32];

            // 15 bits (0-32768) converted to 0-31 (= 5 bits) => shift away the right-most 10 bits.
            // In practice, the values we get are often so small that it makes more sense to retain
            // a few bits more (hence the >> 8) and filter away too-large values.
            int channel_bar_length = (channel_visualizations[i] >> 8) & 0x1F;

            for (int x = 0; x < channel_bar_length; x++)
            {
                bar[x] = 223; // ▀ in CP437
            }

            bar[channel_bar_length] = '\0';

            console_buffer_print(&console_structure, 0, 5 + i, CONSOLE_BUFFER_COLOUR_CYAN, "[");
            console_buffer_print_formatted(&console_structure, 1, 5 + i,
                                            CONSOLE_BUFFER_COLOUR_BRIGHT_CYAN, "%-32s", bar);
            console_buffer_print(&console_structure, 33, 5 + i, CONSOLE_BUFFER_COLOUR_CYAN, "]");
        }

        // Print a status line at the bottom of the screen
        console_buffer_print_formatted(&console_structure, 0, 49, DEFAULT_COLOR,
            " %d Channels, Pos %.3d, Pattern %.3d:%.2d, %.3d BPM, Speed %.3d",
            trackbuf_state.track_state_buf[0].number_of_tracks,
            trackbuf_state.track_state_buf[0].cur_pattern_table_pos,
            trackbuf_state.track_state_buf[0].cur_pattern,
            trackbuf_state.track_state_buf[0].cur_pattern_pos,
            trackbuf_state.track_state_buf[0].bpm,
            trackbuf_state.track_state_buf[0].speed
        );

        // Handle keypresses. This needs to happen late in the function
        // to avoid keypresses having a "laggy" feel, since the audio
        // playback part etc. in this function takes some time.
        bool block = FALSE;
        if (console_event_wait(&console_structure, &keyboard_packet, &event_type, block) == CONSOLE_RETURN_SUCCESS &&
            event_type == CONSOLE_EVENT_KEYBOARD)
        {
            if (keyboard_packet.has_character_code)
            {
                switch (keyboard_packet.character_code[0])
                {
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    {
                        current_mod_file = keyboard_packet.character_code[0] - '1';
                        current_mod_file_selecting = current_mod_file;

                        if (!select_modfile(current_mod_file))
                        {
                            return -1;
                        }

                        break;
                    }
                }
            }
            else if (keyboard_packet.has_special_key && keyboard_packet.key_pressed)
            {
                switch (keyboard_packet.special_key)
                {
                    case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_8:
                    {
                        current_mod_file_selecting -= 1;

                        if (current_mod_file_selecting < 0)
                        {
                            current_mod_file_selecting = 0;
                        }

                        break;
                    }

                    case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_2:
                    {
                        current_mod_file_selecting += 1;

                        if (current_mod_file_selecting >= NUM_MODFILES)
                        {
                            current_mod_file_selecting = NUM_MODFILES - 1;
                        }

                        break;
                    }

                    case IPC_KEYBOARD_SPECIAL_KEY_ENTER:
                    {
                        current_mod_file = current_mod_file_selecting;

                        if (!select_modfile(current_mod_file))
                        {
                            return -1;
                        }

                        break;
                    }
                }
            }
        }

        // Print out information about available modules. We do this
        // after keypresses have been handled to provide a smoother UX.
        for (int i = 0; i < NUM_MODFILES; i++)
        {
            int color = i == current_mod_file ?
                CONSOLE_BUFFER_COLOUR_BRIGHT_WHITE | CONSOLE_BUFFER_BG_COLOUR_BLUE :
                CONSOLE_BUFFER_COLOUR_LIGHT_GRAY;

            if (i == current_mod_file_selecting &&
                i != current_mod_file)
            {
                color = CONSOLE_BUFFER_COLOUR_BRIGHT_WHITE;
            }

            // Abuse the fact that the .mod file begins with the name, hopefully NUL-terminated. :)
            console_buffer_print_formatted(&console_structure, 0, 37 + i,
                                           color, " [%d] %-20s", i + 1, modfiles[i]);
        }

        console_flip(&console_structure);
    }

    return 0;
}

static bool select_modfile(int current_mod_file)
{
    uint8_t *modfile = modfiles[current_mod_file];
    int modfile_size = modfile_sizes[current_mod_file];

    if (!initialize_mod_player(modfile, modfile_size))
    {
        return FALSE;
    }

    print_mod_name();

    return TRUE;
}

static bool initialize_mod_player(uint8_t *modfile, int length)
{
    if (hxcmod_init(&modctx) == 0)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "hxcmod initialization failed");
        return FALSE;
    }

    if (hxcmod_load(&modctx, modfile, length) == 0)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "hxcmod module parsing failed");
        return FALSE;
    }

    return TRUE;
}

static void initialize_tracker_buffer_state(tracker_buffer_state *trackbuf_state)
{
    memory_set_uint8_t((uint8_t *) trackbuf_state, 0, sizeof(tracker_buffer_state));
    trackbuf_state->nb_max_of_state = 1;
    memory_allocate((void **) &trackbuf_state->track_state_buf, sizeof(tracker_state) * trackbuf_state->nb_max_of_state);
    memory_set_uint8_t((uint8_t *) trackbuf_state->track_state_buf, 0,
                       sizeof(tracker_state) * trackbuf_state->nb_max_of_state);
    trackbuf_state->sample_step = NUMBER_OF_SAMPLES / trackbuf_state->nb_max_of_state;
}

static void print_mod_name(void)
{
    console_buffer_print_formatted(&console_structure, 0, 2, DEFAULT_COLOR,
                                   "Playing file: %-20s", modctx.song.title);
}

static uint16_t abs(int16_t value)
{
    if (value < 0)
    {
        // Added case to attempt to handle -32768 => 32768 conversion properly (which is unable
        // to store in a signed int16_t)
        return 0 - (int32_t)value;
    }
    else
    {
        return value;
    }
}
