

#include "nsaeipc.h"

#include <stdio.h>

int
nsae_ipc_init (int mode, char *custom_client, char *custom_server)
{
    (void)fprintf (stderr, "nsae_ipc_init()\n");
    return 0;
}

void
nsae_ipc_free (int mode)
{
    (void)fprintf (stderr, "nsae_ipc_free()\n");
}

int
nsae_ipc_send (void *buf, size_t n)
{
    (void)fprintf (stderr, "nsae_ipc_send()\n");
    return 0;
}

int
nsae_ipc_send_block (void *buf, size_t n)
{
    (void)fprintf (stderr, "nsae_ipc_send_block()\n");
    return 0;
}

int
nsae_ipc_recieve (void *buf, size_t n)
{
    (void)fprintf (stderr, "nsae_ipc_recieve()\n");
    return 0;
}

int
nsae_ipc_recieve_block (void *buf, size_t n)
{
    (void)fprintf (stderr, "nsae_ipc_recieve_block()\n");
    return 0;
}


/* end of file */
