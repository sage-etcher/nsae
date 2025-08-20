
#include "nsae.h"

int
main (int argc, char **argv)
{
    nsae_t nsae = { 0 };
    return nsae_start (&nsae, &argc, argv);
}

/* end of file */
