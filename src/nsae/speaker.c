

#include "speaker.h"

#include "audio.h"
#include "cpu.h"
#include "darray.h"

#include <portaudio.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_RATE         44100
#define FRAMES_PER_BUFFER   AUDIO_BUFFER_SIZE

typedef struct { float quot, rem; } fdiv_t;

fdiv_t fdiv (float x, float y);

static event_time_t event_time_cmp (event_time_t x, event_time_t y);
static int event_time_less_than (event_time_t x, event_time_t y);
static int event_time_in_range (event_time_t x, event_time_t y, 
        event_time_t range);

static float volume_to_amplitude (float volume_level);

static float speaker_generate_amplitude (speaker_t *self);
static void speaker_generate_buffer (speaker_t *self, unsigned long n);

static int speaker_callback (const void *input_buffer, void *output_buffer, 
        unsigned long frames_per_buffer, 
        const PaStreamCallbackTimeInfo *time_info, 
        PaStreamCallbackFlags status_flags, void *user_data);

static float s_cpu_hz = 0;
static float s_cycles_run = 0;
static float s_cycles_per_buffer = 0;
static float s_cycles_per_frame = 0;


fdiv_t
fdiv (float x, float y)
{
    /* {{{ */
    fdiv_t result;

    result.quot = floorf (x / y);
    result.rem  = x - (result.quot * y);

    return result;
    /* }}} */
}

void
speaker_set_cpu_speed (speaker_t *self, float cpu_hz, float cycles_run)
{
    /* {{{ */
    s_cpu_hz = cpu_hz;
    s_cycles_run = cycles_run;

    s_cycles_per_frame = cpu_hz / SAMPLE_RATE;
    s_cycles_per_buffer = s_cycles_per_frame * FRAMES_PER_BUFFER;
    /* }}} */
}

static event_time_t
event_time_cmp (event_time_t x, event_time_t y)
{
    /* {{{ */
    /* return x - y */
    event_time_t carry  = event_time_new (0, 0);
    event_time_t result = event_time_new (0, 0);

    /* handle carry */
    if (x.frame < y.frame)
    {
        carry.frame = 1;
        carry.cycle = -s_cycles_run;
    }

    /* calculate result */
    result.frame = x.frame - y.frame + carry.frame;
    result.cycle = x.cycle - y.cycle + carry.cycle;

    return result;
    /* }}} */
}

static int
event_time_less_than (event_time_t x, event_time_t y)
{
    /* {{{ */
    if (x.frame > y.frame) { return 0; }

    if (x.frame < y.frame) { return 1; }
    if (x.cycle < y.cycle) { return 1; }

    return 0;
    /* }}} */
}

static int
event_time_in_range (event_time_t x, event_time_t y, event_time_t range)
{
    /* {{{ */
    event_time_t diff = event_time_cmp (x, y);
    
    if ((diff.frame > 0) || (diff.frame < -range.frame))
    {
        return 0;
    }

    if ((diff.cycle > 0) || (diff.cycle < -range.cycle))
    {
        return 0;
    }

    return 1;
    /* }}} */
}

int
speaker_init (speaker_t *self)
{
    /* {{{ */
    PaError err = 0;
    
    assert (self != NULL);

    /* default values */
    self->last_state = 0;
    self->time = event_time_new (0, 0);
    self->stream = NULL;
    self->pause = 1;
    self->volume_level = 100.F;

    self->transition_index = 0;
    DA_INIT (self->transitions, event_time_t);

    memset (self->buffer, 0, sizeof (self->buffer));

    /* initialize and open default stream */
    err = Pa_Initialize ();
    if (err != paNoError) 
    { 
        (void)fprintf (stderr, "cannot initialize portaudio\n");
        return 1; 
    }

    err = Pa_OpenDefaultStream (
            &self->stream,
            0,
            1,
            paUInt8,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            speaker_callback,
            self);

    if (err != paNoError) 
    {
        (void)fprintf (stderr, "cannot open stream\n");
        speaker_destroy (self);
        return 1;
    }

    return 0;
    /* }}} */
}

int
speaker_start (speaker_t *self)
{
    /* {{{ */
    PaError err = 0;

    err = Pa_StartStream (self->stream);
    if (err != paNoError)
    {
        (void)fprintf (stderr, "cannot start stream\n");
        return 1;
    }

    return 0;
    /* }}} */
}

int
speaker_stop (speaker_t *self)
{
    /* {{{ */
    PaError err = 0;

    err = Pa_StopStream (self->stream);
    if (err != paNoError)
    {
        (void)fprintf (stderr, "cannot stop stream\n");
        return 1;
    }

    return 0;
    /* }}} */
}

void
speaker_destroy (speaker_t *self)
{
    /* {{{ */
    speaker_stop (self);
    Pa_CloseStream (self->stream);
    Pa_Terminate ();
    return;
    /* }}} */
}

int
speaker_toggle (speaker_t *self, event_time_t event_time)
{
    /* {{{ */
    int i = 0;
    event_time_t diff = event_time_new (0, 0);

    assert (self != NULL);
    
    da_pop_front ((da_void_t *)&self->transitions, (int)self->transition_index);
    self->transition_index = 0;

    da_resize ((da_void_t *)&self->transitions, 1);
    
    /* search linearly backwards for location to insert at */
    for (i = (int)self->transitions.count; i > 0; i--)
    {
        diff = event_time_cmp (self->transitions.m[i-1], event_time);

        if ((diff.frame < 0) || (diff.cycle < 0)) 
        { 
            break; 
        }
    }

    assert (i >= 0);
    assert ((size_t)i <= self->transitions.count);

    da_insert ((da_void_t *)&self->transitions, i);
    self->transitions.m[i] = event_time;

    return 0;
    /* }}} */
}

void 
speaker_generate_frequency (speaker_t *self, float frequency, int ms_duration)
{
    /* {{{ */
    const float SECOND_MS = 1000;
    float cycles_per_toggle = s_cpu_hz / frequency / 2;
    uint32_t toggle_count = s_cpu_hz / cycles_per_toggle * ((float)ms_duration / SECOND_MS);
    uint32_t i = 0; /* NOLINT */
    float unwrapped_cycle = 0;
    fdiv_t result = { 0 };

    event_time_t event_time = event_time_new (0, 0);

    for (i = 0; i < toggle_count; i++)
    {
        unwrapped_cycle = (float)i * cycles_per_toggle + self->time.cycle;
        result = fdiv (unwrapped_cycle, s_cycles_run);

        event_time.frame = (int)result.quot + self->time.frame;
        event_time.cycle = result.rem;

        speaker_toggle (self, event_time);
    }

    return;
    /* }}} */
}

static float
volume_to_amplitude (float volume_level)
{
    const float MAX_VOLUME = 100.F;
    const float MAX_AMPLITUDE = 1.F;
    return MAX_AMPLITUDE * (volume_level / MAX_VOLUME);
}

static int
speaker_callback (const void *input_buffer, void *output_buffer, /* NOLINT */
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags, void *user_data)
{
    /* {{{ */
    speaker_t *self = (speaker_t *)user_data;
    uint8_t *out = output_buffer;
    unsigned long i = 0; /* NOLINT */
    float amplitude = 0.F;
    uint8_t output = 0;

    (void)input_buffer;

    speaker_generate_buffer (self, frames_per_buffer);
    audio_normalize (self->buffer, volume_to_amplitude (self->volume_level));

    for (i = 0; i < frames_per_buffer; i++)
    {
        /* bind amplitude to -1.0 to 1.0 */
        amplitude = fminf (1.F, fmaxf (0.F, self->buffer[i]));

        /* calculate final value */
        output = amplitude * UINT8_MAX;
        //printf ("output: %3u % 7.4f %3.0f\n", 
        //        output, amplitude, self->volume_level);
        *out++ = output;
    }

    return 0;
    /* }}} */
}

static float
speaker_generate_amplitude (speaker_t *self)
{
    /* {{{ */
    size_t i = 0;
    event_time_t *transition = NULL;
    const event_time_t RANGE = { .frame = 0, .cycle = s_cycles_per_frame };

    /* if pause dont output audio */
    if (self->pause)
    {
        return 0.F;
    }

    /* allow speaker to fall back if emulator is running slow */
    if (self->transition_index < self->transitions.count)
    {
        transition = &self->transitions.m[self->transition_index];

        if (event_time_less_than (*transition, self->time))
        {
            self->time = *transition;
        }
    }

    while (self->transition_index < self->transitions.count)
    {
        transition = &self->transitions.m[self->transition_index];

        if (!event_time_in_range (self->time, *transition, RANGE)) 
        {
            break;
        }

        //printf ("match: %8f %8d\n", transition->cycle, transition->frame);
        self->last_state ^= 1;
        self->transition_index++;
    }

    return ((self->last_state) ? 1.F : 0.F);
    /* }}} */
}

static void
speaker_generate_buffer (speaker_t *self, unsigned long n)
{
    /* {{{ */
    unsigned long i = 0;
    fdiv_t result = { 0 };

    for (i = 0; i < n; i++)
    {
        /* generate one */
        self->buffer[i] = speaker_generate_amplitude (self);

        /* increment time */
        if (!self->pause)
        {
            self->time.cycle += s_cycles_per_frame;

            result = fdiv (self->time.cycle, s_cycles_run);
            if (result.quot > 0)
            {
                self->time.cycle  = result.rem;
                self->time.frame += (int)result.quot;
            }
        }
    }
    
    return;
    /* }}} */
}

void
speaker_beep (speaker_t *self)
{
    /* {{{ */
    const float FREQUENCY = 1920.F;
    const int DURATION_MS = 500;
    speaker_generate_frequency (self, FREQUENCY, DURATION_MS);
    /* }}} */
}

/* vim: fdm=marker
 * end of file */

