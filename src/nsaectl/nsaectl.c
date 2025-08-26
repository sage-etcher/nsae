
#include "client.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "nsaectl_help.h"
#include "nsaectl_version.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define PROJ_VERSION "0.1.0"

#define VERSION_STR SRC_NSAECTL_NSAECTL_VERSION
#define VERSION_LEN SRC_NSAECTL_NSAECTL_VERSION_LEN

#define USAGE_STR ___SRC_NSAECTL_NSAECTL_HELP
#define USAGE_LEN ___SRC_NSAECTL_NSAECTL_HELP_LEN


int
main (int argc, char **argv)
{
    int opt = 0;

    bool verbose = false;
    char *custom_server = NULL;
    char *custom_client = NULL;

    /* global options */
    while ((opt = getopt (argc, argv, "f:F:htvV")) != -1)
    {
        switch (opt)
        {
        case 'F': /* custom client fifo locaction */
            custom_client = optarg;
            break;

        case 'f': /* custom server fifo location */
            custom_server = optarg;
            break;

        case 't': /* terse */
            verbose = false;
            break;

        case 'v': /* verbose */
            verbose = true;
            break;

        case 'V': /* version */
            fprintf (stderr, "%.*s", VERSION_LEN, VERSION_STR);
            exit (EXIT_SUCCESS);

        case 'h': /* help */
            fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
            exit (EXIT_SUCCESS);

        default: /* error */
            fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
            exit (EXIT_FAILURE);
        }
    }

    /* mode selection */
    const char *mode_list[NSAE_CMD_COUNT] = {
        [NSAE_CMD_EXIT]       = "exit",
        [NSAE_CMD_RESTART]    = "restart",
        [NSAE_CMD_PAUSE]      = "pause",
        [NSAE_CMD_CONTINUE]   = "continue",
        [NSAE_CMD_EJECT_FD]   = "fd_eject",
        [NSAE_CMD_LOAD_FD]    = "fd_load",
        [NSAE_CMD_SAVE_FD]    = "fd_save",
        [NSAE_CMD_EJECT_HD]   = "hd_eject",
        [NSAE_CMD_LOAD_HD]    = "hd_load",
        [NSAE_CMD_SAVE_HD]    = "hd_save",
        [NSAE_CMD_STEP]       = "step",
        [NSAE_CMD_RUN]        = "run",
        [NSAE_CMD_BREAKPOINT] = "breakpoint",
        [NSAE_CMD_STATUS]     = "status",
    };

    argc -= optind;
    argv += optind;

    if (argc <= 0)
    {
        fprintf (stderr, "nsaectl: missing field -- mode\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    const char *mode_str = argv[0];
    int mode = 0;
    for (; mode < NSAE_CMD_COUNT; mode++)
    {
        if (0 == strcmp (mode_list[mode], mode_str)) 
        {
            break;
        }
    }

    assert (mode <= NSAE_CMD_COUNT);
    if (mode == NSAE_CMD_COUNT)
    {
        fprintf (stderr, "nsaectl: invalid mode -- \"%s\"\n", mode_str);
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    int mode_argc[NSAE_CMD_COUNT] = {
        [NSAE_CMD_EXIT]       = 1,
        [NSAE_CMD_RESTART]    = 1,
        [NSAE_CMD_PAUSE]      = 1,
        [NSAE_CMD_CONTINUE]   = 1,
        [NSAE_CMD_EJECT_FD]   = 2,
        [NSAE_CMD_LOAD_FD]    = 3,
        [NSAE_CMD_SAVE_FD]    = 3,
        [NSAE_CMD_EJECT_HD]   = 1,
        [NSAE_CMD_LOAD_HD]    = 2,
        [NSAE_CMD_SAVE_HD]    = 2,
        [NSAE_CMD_STEP]       = 1,
        [NSAE_CMD_RUN]        = 1,
        [NSAE_CMD_BREAKPOINT] = 2,
        [NSAE_CMD_STATUS]     = 1,
    };

    if (argc != mode_argc[mode])
    {
        fprintf (stderr, "nsaectl: missing mode argument\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    int rc = nsae_ipc_init (NSAE_IPC_CLIENT, custom_client, custom_server);
    if (rc != 0)
    {
        fprintf (stderr, "nsaectl: failed to initialize ipc\n");
        exit (EXIT_FAILURE);
    }


    switch (mode)
    {
    case NSAE_CMD_BREAKPOINT: /* cmd addr */
    case NSAE_CMD_EJECT_FD:   /* cmd fd_num */
        send_cmd (mode, atoi (argv[1]));
        break;

    /* cmd fd_num len filename */
    case NSAE_CMD_LOAD_FD:
    case NSAE_CMD_SAVE_FD:
        send_cmd (mode, atoi (argv[1]), argv[2]);
        break;

    /* cmd len filename */
    case NSAE_CMD_LOAD_HD:
    case NSAE_CMD_SAVE_HD:
        send_cmd (mode, argv[1]);
        break;

    /* cmd */
    case NSAE_CMD_CONTINUE:
    case NSAE_CMD_EJECT_HD:
    case NSAE_CMD_EXIT:
    case NSAE_CMD_PAUSE:
    case NSAE_CMD_RESTART:
    case NSAE_CMD_RUN:
    case NSAE_CMD_STATUS:
    case NSAE_CMD_STEP:
        send_cmd (mode);
        break;

    /* error */
    default: 
        abort ();
    }

    nsae_ipc_free (NSAE_IPC_CLIENT);

    return 0;
}


/* end of file */
