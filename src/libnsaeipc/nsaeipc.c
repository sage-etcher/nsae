
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

#define NSAE_IPC_CLIENT_FILE "nsae_client_fifo"
#define NSAE_IPC_SERVER_FILE "nsae_server_fifo"

static const char *nsae_fifo_path (void);
static char *path_concat (const char *parent, const char *child);
static int open_fifo (char *filename, int o_mode, int o_cancel_mode);

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

        remove (s_client_fifo);
        remove (s_server_fifo);

        mkfifo (s_client_fifo, 0600);
        mkfifo (s_server_fifo, 0600);
    }

    switch (mode)
    {
    case NSAE_IPC_SERVER:
        s_client_fd = open_fifo (s_client_fifo, O_WRONLY, O_RDONLY);
        s_server_fd = open (s_server_fifo, O_RDONLY | O_NONBLOCK);
        s_send    = &s_client_fd;
        s_recieve = &s_server_fd;
        break;

    case NSAE_IPC_CLIENT:
        s_client_fd = open_fifo (s_client_fifo, O_RDONLY, O_WRONLY);
        s_server_fd = open_fifo (s_server_fifo, O_WRONLY, O_RDONLY);
        s_send    = &s_server_fd;
        s_recieve = &s_client_fd;
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

        remove (s_client_fifo);
        remove (s_server_fifo);
    }

    free (s_client_fifo);
    free (s_server_fifo);

    s_client_fifo = NULL;
    s_server_fifo = NULL;
}

int
nsae_ipc_send (void *buf, size_t n)
{
    int rc = 0;
    if (s_send == NULL) return -1;
    if (buf == NULL) return -1;
    rc |= write (*s_send, &n, sizeof (n));
    rc |= write (*s_send, buf, n);
    return rc;
}

int
nsae_ipc_recieve (void *buf, size_t n)
{
    if (s_recieve == NULL) return -1;
    if (buf == NULL) return -1;
    int rc = read (*s_recieve, buf, n);
    return rc;
}

int
nsae_ipc_recieve_block (void *buf, size_t n)
{
    if (s_recieve == NULL) return -1;
    if (buf == NULL) return -1;

    int b = 0;
    while ((b = read (*s_recieve, buf, n)) == 0) { }
    return b;
}

int
nsae_ipc_send_block (void *buf, size_t n)
{
    if (s_send == NULL) return -1;
    if (buf == NULL) return -1;

    int b = 0;
    while ((b = write (*s_send, buf, n)) == 0) 
    { 
        printf ("%d\n", b);
    }
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

static int 
open_fifo (char *filename, int o_mode, int o_cancel_mode)
{
    /* by default a blocking fifo will block on open until both read and write
     * ends are opened. this is non-ideal.
     * this workaround forks the process and allows the caller to open both
     * heads then discard the latter unecessary one. in turn this stops the
     * open lock even on blocking opens */

    int pid = fork();
    if (pid > 0) /* parent */
    {
        return open (filename, o_mode);
    }
    else if (pid == 0) /* cihld */
    {
        int fifo_fd = open (filename, o_cancel_mode);
        close (fifo_fd);
        exit (0);
    }
    else if (pid < 0) /* error */
    {
        fprintf (stderr, "libnsaeipc: failed to fork\n");
        return -1;
    }

    /* unreachable */
    return -1;
}



/* end of file */
