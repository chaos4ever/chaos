/* Little John, a NES emulator.
   File : unes_apu.h
   Authors : Yoyo.
   Version : 0.3
   Last update : 3rd Mai 2000 */

#ifndef __UNES_APU_H__
#define __UNES_APU_H__

#include <system/system.h>

void APU_Access(word Addr,byte Value);
void Sound_CallBack(void *udata, u8 *stream, int len);
void Sound_Update(int synchro);

#define SndBuffDiff 256/8

#define SndBuffSize 32768

long FrequencyMagicSquare,FrequencyMagicNoise,FrequencyMagicTriangle;
//thanks to darcnes

byte channel1,channel2,channel3,channel4,channel5,channel6;

byte Pulse1_ctrl1,Pulse1_ctrl2,Pulse1_vol,Pulse1_reg3,Pulse1_reg4,Pulse1_active;
long Pulse1_freq,Pulse1_length,Pulse1_env_phase,Pulse1_env_delay,Pulse1_sweep_phase,Pulse1_sweep_delay;
unsigned long Pulse1_samplepos;

byte Pulse2_ctrl1,Pulse2_ctrl2,Pulse2_vol,Pulse2_reg3,Pulse2_reg4,Pulse2_active;
long Pulse2_freq,Pulse2_length,Pulse2_env_phase,Pulse2_env_delay,Pulse2_sweep_phase,Pulse2_sweep_delay;
unsigned long Pulse2_samplepos;

byte Pulse3_ctrl1,Pulse3_ctrl2,Pulse3_reg3,Pulse3_reg4,Pulse3_active,Pulse3_write_latency,Pulse3_cnt_started;
long Pulse3_freq,Pulse3_length,Pulse3_linear_length;
unsigned long Pulse3_samplepos;


byte Noise_ctrl1,Noise_ctrl2,Noise_vol,Noise_reg3,Noise_reg4,Noise_active;
long Noise_freq,Noise_length,Noise_env_phase,Noise_env_delay;
unsigned long Noise_samplepos;
byte noise_lut[0x4000];
float noise_phaseacc;

byte dpcm_irq,dpcm_active,dpcm_reg1,dpcm_reg3,dpcm_reg4,dpcm_cur_byte;
long dpcm_freq,dpcm_length,dpcm_address,dpcm_bits_len,dpcm_reg2;
float dpcm_phaseacc;

byte extra_active;

float apu_len2mix;
long apu_buf_size;
float apu_inc_size;//,apu_cycle;

byte apu_ctrl;
byte *audio_buffer[6];
byte *audio_buffer_final;
byte prev_sampleL,next_sampleL,prev_sampleR,next_sampleR;

typedef struct { byte addr; byte data; long cycle; } apu_event;
apu_event apu_event_log[1024];
int apu_event_cnt;

#endif /* !__UNES_APU_H__ */
