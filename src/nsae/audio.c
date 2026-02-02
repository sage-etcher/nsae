

#include "audio.h"

#include <math.h>


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

/* scale_decibel is a positive value to scale all frames of data by */
void
audio_amplify (float buffer[], float scale_decibel)
{
    int i = 0; /* NOLINT */
    float tmp_amplitude = 0;
    float tmp_decibel = 0;


    for (i = 0; i < AUDIO_BUFFER_SIZE; i++)
    {
        tmp_decibel = amplitude_to_decibel (buffer[i]) / scale_decibel;
        tmp_amplitude = decibel_to_amplitude (tmp_decibel);
        if (buffer[i] > 0)
        {
            tmp_amplitude *= -1;
        }

        buffer[i] = tmp_amplitude;
    }
}

void
audio_normalize (float buffer[], float target_decibel)
{
    float peak = amplitude_to_decibel (audio_get_peak (buffer));
    float scale = target_decibel / peak;
    audio_amplify (buffer, scale);
}


/* end of file */
