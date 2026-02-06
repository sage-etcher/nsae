
#include "nsaeipc.h"

#include <zmq.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void my_free (void *data, void *hint);

void *s_context = NULL;
void *s_socket = NULL;

int
nsae_ipc_init_server (const char *responder_addr)
{
    s_context = zmq_ctx_new ();
    s_socket = zmq_socket (s_context, ZMQ_REP);
    zmq_connect (s_socket, responder_addr); 

    return 0;
}

int
nsae_ipc_init_client (const char *requester_addr)
{
    int rc = 0;

    s_context = zmq_ctx_new ();
    s_socket = zmq_socket (s_context, ZMQ_REQ);
    rc = zmq_bind (s_socket, requester_addr); 
    assert (rc == 0);

    return 0;
}

void
nsae_ipc_quit (void)
{
    zmq_close (s_socket);
    zmq_ctx_destroy (s_context);

    s_context = NULL;
    s_socket = NULL;
}

int
nsae_ipc_send (int flags, void *buf, size_t n)
{
    int rc = 0;
    int errno_bak = 0;
    void *copy = NULL;
    zmq_msg_t msg;

    assert (buf != NULL);
    assert (n != 0);

    copy = malloc (n);
    assert (copy != NULL);
    memcpy (copy, buf, n);

    rc = zmq_msg_init_data (&msg, copy, n, my_free, NULL);
    assert (rc == 0);

    rc = zmq_msg_send (&msg, s_socket, flags);
    errno_bak = errno;
    if (rc == -1)
    {
        zmq_msg_close (&msg);
        free (copy);
    }

    errno = errno_bak;
    return rc;
}

int
nsae_ipc_recieve (int flags, void **p_buf, size_t *p_n)
{
    int rc = 0;
    int errno_bak = 0;
    size_t n = 0;
    void *copy = NULL;
    zmq_msg_t msg;

    assert (p_buf != NULL);
    assert (p_n != NULL);

    zmq_msg_init (&msg);
    rc = zmq_msg_recv (&msg, s_socket, flags);
    errno_bak = errno;
    if (rc == -1)
    {
        zmq_msg_close (&msg);
        errno = errno_bak;
        return 1;
    }

    n = zmq_msg_size (&msg);
    if (n == 0)
    {
        zmq_msg_close (&msg);
        errno = errno_bak;
        return 1;
    }

    copy = malloc (n);
    assert (copy != NULL);
    memcpy (copy, zmq_msg_data (&msg), n);

    zmq_msg_close (&msg);

    *p_buf = copy;
    *p_n = n;
    errno = errno_bak;
    return 0;

}

static void
my_free (void *data, void *hint)
{
    (void)hint;
    free (data);
}


/* end of file */
