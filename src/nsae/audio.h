
#ifndef AUDIO_H
#define AUDIO_H

#define AUDIO_BUFFER_SIZE 512

/* conversions */
float amplitude_to_decibel (float amplitude);
float decibel_to_amplitude (float decibel);

/* amplitude */
float audio_get_peak  (float buffer[]);

/* decibel */
void  audio_amplify   (float buffer[], float scale_amplitude);
void  audio_normalize (float buffer[], float target_amplitude);


#endif
/* end of file */

