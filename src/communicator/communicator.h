
#ifndef NSAEIPC_H
#define NSAEIPC_H

#include <zmq.h>

#include <stddef.h>
#include <stdint.h>

int nsae_ipc_init_server (const char *custom_addr);
int nsae_ipc_init_client (const char *custom_addr);
void nsae_ipc_quit (void);

int nsae_ipc_send (int flags, void *buf, size_t n);
int nsae_ipc_recieve (int flags, void **p_buf, size_t *p_n);

#endif /* NSAEIPC_H */
