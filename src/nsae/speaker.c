
#include "speaker.h"

#include "log.h"

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

    log_info ("TODO: speaker BEEEEEEEEP!!\n");
}


/* end of file */
