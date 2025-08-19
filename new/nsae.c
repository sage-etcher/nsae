
#include "nsae.h"

int
nsae_init (nsae_t *self)
{
    assert (self != NULL);

    int rc = 0;
    rc |= adv_init (&self->adv);
    
    if (rc != 0)
    {
        return 1;
    }

    self->width = 640;
    self->height = 480;
    self->max_fps = 60;

    self->pause = false;
    self->exit = false;

    return 0;
}

int
nsae_start (nsae_t *self)
{
    return 0;
}

/* end of file */
