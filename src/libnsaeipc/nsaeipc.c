
#include "nsaeipc.h"

#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NSAE_IPC_CLIENT_FILE "nsae_send_fifo"
#define NSAE_IPC_SERVER_FILE "nsae_recieve_fifo"

static const char *nsae_fifo_path (void);
static char *path_concat (const char *parent, const char *child);

static char *s_client_fifo = NULL;
static char *s_server_fifo = NULL;

static int s_client_fd = 0;
static int s_server_fd = 0;

static int *s_send = NULL;
static int *s_recieve = NULL;



int
nsae_ipc_init (int mode, char *custom_client, char *custom_server)
{
    const char *path = nsae_fifo_path ();

    /* client fifo */
    if (custom_client == NULL)
    {
        s_client_fifo = path_concat (path, NSAE_IPC_CLIENT_FILE);
    }
    else
    {
        s_client_fifo = custom_client;
    }

    /* server fifo */
    if (custom_server == NULL)
    {
        s_server_fifo = path_concat (path, NSAE_IPC_SERVER_FILE);
    }
    else
    {
        s_server_fifo = custom_server;
    }

    if (mode == NSAE_IPC_SERVER)
    {
        unlink (s_client_fifo);
        unlink (s_server_fifo);

        mkfifo (s_client_fifo, 0600);
        mkfifo (s_server_fifo, 0600);
    }

    switch (mode)
    {
    case NSAE_IPC_CLIENT:
        s_client_fd = open (s_client_fifo, O_RDONLY);
        s_server_fd = open (s_server_fifo, O_WRONLY);
        s_send = &s_server_fd;
        s_recieve = &s_client_fd;
        break;

    case NSAE_IPC_SERVER:
        s_client_fd = open (s_client_fifo, O_WRONLY);
        s_server_fd = open (s_server_fifo, O_RDONLY | O_NONBLOCK);
        s_send = &s_client_fd;
        s_recieve = &s_server_fd;
        break;

    default:
        return 1;
    }

    if (s_client_fd == -1)
    {
        fprintf (stderr, "libnsaeipc: cannot open client fifo\n");
        nsae_ipc_free (mode);
        return 1;
    }

    if (s_server_fd == -1)
    {
        fprintf (stderr, "libnsaeipc: cannot open server fifo\n");
        nsae_ipc_free (mode);
        return 1;
    }

    return 0;
}

void
nsae_ipc_free (int mode)
{
    close (s_client_fd);
    close (s_server_fd);

    s_client_fd = 0;
    s_server_fd = 0;

    s_send = NULL;
    s_recieve = NULL;

    if (mode == NSAE_IPC_SERVER)
    {
        unlink (s_client_fifo);
        unlink (s_server_fifo);
    }

    free (s_client_fifo);
    free (s_server_fifo);

    s_client_fifo = NULL;
    s_server_fifo = NULL;
}

int
nsae_ipc_send (uint8_t buf[], size_t n)
{
    if (s_send == NULL) return -1;
    int rc = write (*s_send, buf, n);
    return rc;
}

int
nsae_ipc_recieve (uint8_t buf[], size_t n)
{
    if (s_recieve == NULL) return -1;
    int rc = read (*s_recieve, buf, n);
    return rc;
}

int
nsae_ipc_recieve_block (uint8_t buf[], size_t n)
{
    if (s_recieve == NULL) return -1;

    int b = 0;
    while ((b = read (*s_recieve, buf, n)) == 0) { }
    return b;
}

static const char *
nsae_fifo_path (void)
{
    const char *xdg_runtime_dir = getenv ("XDG_RUNTIME_DIR");
    const char *fallback = "/tmp";

    if (xdg_runtime_dir != NULL)
    {
        return xdg_runtime_dir;
    }
    else
    {
        fprintf (stderr, "nsae: warning: XDG_RUNTIME_DIR is undefined\n");
        return fallback;
    }
}

static char *
path_concat (const char *parent, const char *child)
{
    assert (parent != NULL);
    assert (child != NULL);

    size_t parent_len = strlen (parent);
    size_t child_len  = strlen (child);

    size_t n = parent_len + child_len + sizeof ('/');
    char *path = malloc (n+1);
    if (path == NULL) return NULL;

    *path = '\0';
    (void)strcat (path, parent);
    (void)strcat (path, "/");
    (void)strcat (path, child);

    path[n] = '\0';

    return path;
}

/* end of file */
