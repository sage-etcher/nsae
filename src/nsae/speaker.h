
#ifndef SPEAKER_H
#define SPEAKER_H

#include "audio.h"
#include "darray.h"

#include <portaudio.h>

#include <stdint.h>

typedef struct {
    float cycle;
    int32_t frame;
} event_time_t;

typedef DARRAY_T (event_time_t) event_timeline_t;

typedef struct {
    PaStream *stream;
    float buffer[AUDIO_BUFFER_SIZE];
    
    event_timeline_t transitions;
    size_t transition_index;

    int pause;
    int last_state;
    event_time_t time; 

    float volume_level;
} speaker_t;

static inline event_time_t event_time_new (int32_t cycle, int32_t frame);

static inline event_time_t 
event_time_new (int32_t cycle, int32_t frame)
{
    /* {{{ */
    return (event_time_t){
        .cycle = cycle,
        .frame = frame,
    };
    /* }}} */
}

void speaker_set_cpu_speed (speaker_t *self, float cpu_hz, float cycles_run);

int  speaker_init    (speaker_t *self);
int  speaker_start   (speaker_t *self);
int  speaker_stop    (speaker_t *self);
void speaker_send_stop_start (speaker_t *self, int stop_flag);
void speaker_destroy (speaker_t *self);

int speaker_toggle (speaker_t *self, event_time_t event_time);
void speaker_generate_frequency (speaker_t *self, float frequency, int ms_duration);

void speaker_beep (speaker_t *self);

void speaker_status (speaker_t *self);


#endif
/* end of file */
