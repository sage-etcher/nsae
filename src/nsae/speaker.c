
#include "speaker.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int
speaker_init (speaker_t *self)
{
    assert (self != NULL);

    return 0;
}

void
speaker_beep (speaker_t *self)
{
    assert (self != NULL);

    fprintf (stdout, "TODO: speaker BEEEEEEEEP!!\n");
}


/* end of file */
