
#ifndef NSAEIPC_H
#define NSAEIPC_H

#include <stddef.h>
#include <stdint.h>


enum {
    NSAE_IPC_CLIENT,
    NSAE_IPC_SERVER,
};


int  nsae_ipc_init (int mode);
void nsae_ipc_free (int mode);

int nsae_ipc_send (uint8_t buf[], size_t n);
int nsae_ipc_recieve (uint8_t buf[], size_t n);

#endif /* NSAEIPC_H */
