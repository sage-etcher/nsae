
#include "nsae.h"

int
main (int argc, char **argv)
{
    nsae_t *emu = NULL;

    /* initialize */
    emu = nsae_new ();
    if (emu == NULL)
    {
        fprintf ("failed to initialize emulator\n");
        return 1;
    }

    /* run */
    nsae_start (emu);

    /* clean up */
    nsae_free (emu);
    return 0;
}

/* end of file */
