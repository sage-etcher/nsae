

#include "audio.h"

#include <math.h>
#include <stdio.h>


float
amplitude_to_decibel (float amplitude)
{
    const float FACTOR = 20.F;


    return FACTOR * log10f (fabsf (amplitude));
}

float
decibel_to_amplitude (float decibel)
{
    const float FACTOR = 20.F;
    const float BASE = 10.F;
    return powf (BASE, decibel / FACTOR);
}


float
audio_get_peak (float buffer[])
{
    int i = 0; /* NOLINT */
    float peak_amplitude = 0;

    /* find the peak amplitude */
    for (i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        float absolute_amplitude = fabsf (buffer[i]);
        if (absolute_amplitude > peak_amplitude)
        {
            peak_amplitude = absolute_amplitude;
        }
    }

    return peak_amplitude;
}

void
audio_amplify (float buffer[], float scale_amplitude)
{
    int i = 0; /* NOLINT */

    for (i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        buffer[i] /= scale_amplitude;
    }
}

void
audio_normalize (float buffer[], float target_amplitude)
{
    float peak  = audio_get_peak (buffer);
    float scale = peak / target_amplitude;
    audio_amplify (buffer, scale);
}


/* end of file */
