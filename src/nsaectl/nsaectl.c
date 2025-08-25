
#include "nsaeipc.h"
#include "nsaecmd.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char **argv)
{
    const char *cmds[NSAE_CMD_COUNT] = {
        [NSAE_CMD_EXIT]       = "exit",
        [NSAE_CMD_RESTART]    = "restart",
        [NSAE_CMD_PAUSE]      = "pause",
        [NSAE_CMD_CONTINUE]   = "continue",
        [NSAE_CMD_EJECT_FD]   = "eject_fd",
        [NSAE_CMD_LOAD_FD]    = "load_fd",
        [NSAE_CMD_SAVE_FD]    = "save_fd",
        [NSAE_CMD_EJECT_HD]   = "eject_hd",
        [NSAE_CMD_LOAD_HD]    = "load_hd",
        [NSAE_CMD_SAVE_HD]    = "save_hd",
        [NSAE_CMD_STEP]       = "step",
        [NSAE_CMD_RUN]        = "run",
        [NSAE_CMD_BREAKPOINT] = "breakpoint",
        [NSAE_CMD_STATUS]     = "status",
    };

    int cmd_min_args[NSAE_CMD_COUNT] = {
        [NSAE_CMD_EXIT]       = 2,
        [NSAE_CMD_RESTART]    = 2,
        [NSAE_CMD_PAUSE]      = 2,
        [NSAE_CMD_CONTINUE]   = 2,
        [NSAE_CMD_EJECT_FD]   = 3,
        [NSAE_CMD_LOAD_FD]    = 4,
        [NSAE_CMD_SAVE_FD]    = 4,
        [NSAE_CMD_EJECT_HD]   = 2,
        [NSAE_CMD_LOAD_HD]    = 3,
        [NSAE_CMD_SAVE_HD]    = 3,
        [NSAE_CMD_STEP]       = 2,
        [NSAE_CMD_RUN]        = 2,
        [NSAE_CMD_BREAKPOINT] = 3,
        [NSAE_CMD_STATUS]     = 2,
    };

    uint8_t cmd = 0;
    for (; cmd < NSAE_CMD_COUNT; cmd++)
    {
        if (0 == strcmp (argv[1], cmds[cmd])) break;
    }

    if ((cmd == NSAE_CMD_COUNT) || (cmd_min_args[cmd] != argc))
    {
        fprintf (stderr, 
                "usage: nsaectl mode [mode_data...]\n"
                "\n"
                "modes:\n"
                "  exit\n"
                "  restart\n"
                "  pause\n"
                "  continue\n"
                "  eject_fd <fd_num>\n"
                "  load_fd <fd_num> <file>\n"
                "  save_fd <fd_num> <file>\n"
                "  eject_hd\n"
                "  load_hd <file>\n"
                "  save_hd <file>\n"
                "  step\n"
                "  run\n"
                "  breakpoint <abs_address>\n"
                "  status\n"
        );
        return 1;
    }

    nsae_ipc_init (NSAE_IPC_CLIENT);

    /* cmd */
    nsae_ipc_send ((uint8_t *)&cmd, sizeof (uint8_t));

    uint8_t fd_num = 0;
    size_t filename_len = 0;
    char *filename = NULL;
    uint32_t abs_addr = 0;

    switch (cmd)
    {
    case NSAE_CMD_EJECT_FD:
        /* fd_num */
        fd_num = strtol (argv[2], NULL, 10);
        nsae_ipc_send ((uint8_t *)&fd_num, sizeof (uint8_t));
        break;

    case NSAE_CMD_LOAD_FD:
    case NSAE_CMD_SAVE_FD:
        /* fd_num filename_len file */
        fd_num = strtol (argv[2], NULL, 10);
        filename = argv[3];
        filename_len = strlen (filename)+1;
        nsae_ipc_send ((uint8_t *)&fd_num, sizeof (uint8_t));
        nsae_ipc_send ((uint8_t *)&filename_len, sizeof (size_t));
        nsae_ipc_send ((uint8_t *)filename, filename_len);
        break;

    case NSAE_CMD_LOAD_HD:
    case NSAE_CMD_SAVE_HD:
        /* filename_len file */
        filename = argv[2];
        filename_len = strlen (filename)+1;
        nsae_ipc_send ((uint8_t *)&filename_len, sizeof (size_t));
        nsae_ipc_send ((uint8_t *)filename, filename_len);
        break;

    case NSAE_CMD_BREAKPOINT:
        /* abs_addr */
        abs_addr = strtol (argv[2], NULL, 16);
        nsae_ipc_send ((uint8_t *)&abs_addr, sizeof (uint32_t));
        break;

    case NSAE_CMD_EXIT:
    case NSAE_CMD_RESTART:
    case NSAE_CMD_PAUSE:
    case NSAE_CMD_CONTINUE:
    case NSAE_CMD_EJECT_HD:
    case NSAE_CMD_STEP:
    case NSAE_CMD_RUN:
    case NSAE_CMD_STATUS:
    default:
        break;
    }

    nsae_ipc_free (NSAE_IPC_CLIENT);

    return 0;
}

/* end of file */
