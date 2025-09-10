
#define LOG_CATEGORY LC_GENERAL
#include "client.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "nsaectl_help.h"
#include "nsaectl_version.h"
#include "nslog.h"

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

typedef struct {
    uint8_t cmd;
    int min_args;
    int max_args;
} concmd_t;

void
build_dictionary (void)
{
    dict_t *hash = dict_new (NSAE_CMD_COUNT);

    dict_set ("exit",          concmd_new (NSAE_CMD_EXIT,          0, 0));
    dict_set ("restart",       concmd_new (NSAE_CMD_RESTART,       0, 0));
    dict_set ("pause",         concmd_new (NSAE_CMD_PAUSE,         0, 0));
    dict_set ("continue",      concmd_new (NSAE_CMD_CONTINUE,      0, 0));
    dict_set ("brkpnt_set",    concmd_new (NSAE_CMD_BRKPNT_SET,    0, 0));
    dict_set ("brkpnt_remove", concmd_new (NSAE_CMD_BRKPNT_REMOVE, 0, 0));
    dict_set ("brkpnt_list",   concmd_new (NSAE_CMD_BRKPNT_LIST,   0, 0));
    dict_set ("step",          concmd_new (NSAE_CMD_STEP,          0, 0));
    dict_set ("run",           concmd_new (NSAE_CMD_RUN,           0, 0));
    dict_set ("status",        concmd_new (NSAE_CMD_STATUS,        0, 0));
    dict_set ("log_cpu",       concmd_new (NSAE_CMD_LOG_CPU,       0, 0));
    dict_set ("log_mmu",       concmd_new (NSAE_CMD_LOG_MMU,       0, 0));
    dict_set ("log_ram",       concmd_new (NSAE_CMD_LOG_RAM,       0, 0));
    dict_set ("log_fdc",       concmd_new (NSAE_CMD_LOG_FDC,       0, 0));
    dict_set ("log_crt",       concmd_new (NSAE_CMD_LOG_CRT,       0, 0));
    dict_set ("log_kb",        concmd_new (NSAE_CMD_LOG_KB,        0, 0));
    dict_set ("log_mobo",      concmd_new (NSAE_CMD_LOG_MOBO,      0, 0));
    dict_set ("log_terse",     concmd_new (NSAE_CMD_LOG_TERSE,     0, 0));
    dict_set ("log_verbose",   concmd_new (NSAE_CMD_LOG_VERBOSE,   0, 0));
    dict_set ("log_debug",     concmd_new (NSAE_CMD_LOG_DEBUG,     0, 0));
    dict_set ("log_output",    concmd_new (NSAE_CMD_LOG_OUTPUT,    0, 0));
    dict_set ("fd_eject",      concmd_new (NSAE_CMD_FD_EJECT,      0, 0));
    dict_set ("fd_load",       concmd_new (NSAE_CMD_FD_LOAD,       0, 0));
    dict_set ("fd_save",       concmd_new (NSAE_CMD_FD_SAVE,       0, 0));
    dict_set ("fd_blk_read",   concmd_new (NSAE_CMD_FD_BLK_READ, 0, 0));
    dict_set ("fd_status",     concmd_new (NSAE_CMD_FD_STATUS, 0, 0));
    dict_set ("hd_eject",      concmd_new (NSAE_CMD_HD_EJECT, 0, 0));
    dict_set ("hd_load",       concmd_new (NSAE_CMD_HD_LOAD, 0, 0));
    dict_set ("hd_save",       concmd_new (NSAE_CMD_HD_SAVE, 0, 0));
    dict_set ("hd_status",     concmd_new (NSAE_CMD_HD_STATUS, 0, 0));
    dict_set ("kb_push",       concmd_new (NSAE_CMD_KB_PUSH, 0, 0));
    dict_set ("kb_pop",        concmd_new (NSAE_CMD_KB_POP, 0, 0));
    dict_set ("kb_overflow",   concmd_new (NSAE_CMD_KB_OVERFLOW, 0, 0));
    dict_set ("kb_caps",       concmd_new (NSAE_CMD_KB_CAPS, 0, 0));
    dict_set ("kb_cursor",     concmd_new (NSAE_CMD_KB_CURSOR, 0, 0));
    dict_set ("kb_data",       concmd_new (NSAE_CMD_KB_DATA, 0, 0));
    dict_set ("kb_interupt",   concmd_new (NSAE_CMD_KB_INTERUPT, 0, 0));
    dict_set ("kb_status",     concmd_new (NSAE_CMD_KB_STATUS, 0, 0));
    dict_set ("io_status",     concmd_new (NSAE_CMD_IO_STATUS, 0, 0));
    dict_set ("crt_status",    concmd_new (NSAE_CMD_CRT_STATUS, 0, 0));
    dict_set ("adv_out",       concmd_new (NSAE_CMD_ADV_OUT, 0, 0));
    dict_set ("adv_in",        concmd_new (NSAE_CMD_ADV_IN, 0, 0));
    dict_set ("adv_status",    concmd_new (NSAE_CMD_ADV_STATUS, 0, 0));
    dict_set ("cpu_status",    concmd_new (NSAE_CMD_CPU_STATUS, 0, 0));
    dict_set ("ram_read",      concmd_new (NSAE_CMD_RAM_READ, 0, 0));
    dict_set ("ram_write",     concmd_new (NSAE_CMD_RAM_WRITE, 0, 0));
    dict_set ("prom_load",     concmd_new (NSAE_CMD_PROM_LOAD, 0, 0));
    dict_set ("mmu_read",      concmd_new (NSAE_CMD_MMU_READ, 0, 0));
    dict_set ("mmu_write",     concmd_new (NSAE_CMD_MMU_WRITE, 0, 0));
    dict_set ("mmu_load",      concmd_new (NSAE_CMD_MMU_LOAD, 0, 0));
    dict_set ("mmu_status",    concmd_new (NSAE_CMD_MMU_STATUS, 0, 0));


}




int
main (int argc, char **argv)
{
    int opt = 0;
    char *custom_server = NULL;
    char *custom_client = NULL;

    log_init (LC_COUNT);

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
            log_set (LOG_INFO);
            break;

        case 'v': /* verbose */
            log_set (LOG_VERBOSE);
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
        [NSAE_CMD_NOP]           = "nop",
        [NSAE_CMD_EXIT]          = "exit",
        [NSAE_CMD_RESTART]       = "restart",
        [NSAE_CMD_PAUSE]         = "pause",
        [NSAE_CMD_CONTINUE]      = "continue",
        [NSAE_CMD_BRKPNT_SET]    = "br_set",
        [NSAE_CMD_BRKPNT_REMOVE] = "br_remove",
        [NSAE_CMD_BRKPNT_LIST]   = "br_list",
        [NSAE_CMD_STEP]          = "step",
        [NSAE_CMD_RUN]           = "run",
        [NSAE_CMD_STATUS]        = "status",

        [NSAE_CMD_LOG_CPU]       = "log_cpu",
        [NSAE_CMD_LOG_MMU]       = "log_mmu",
        [NSAE_CMD_LOG_RAM]       = "log_ram",
        [NSAE_CMD_LOG_FDC]       = "log_fdc",
        [NSAE_CMD_LOG_CRT]       = "log_crt",
        [NSAE_CMD_LOG_KB]        = "log_kb",
        [NSAE_CMD_LOG_MOBO]      = "log_mobo",
        [NSAE_CMD_LOG_TERSE]     = "log_terse",
        [NSAE_CMD_LOG_VERBOSE]   = "log_verbose",
        [NSAE_CMD_LOG_DEBUG]     = "log_debug",
        [NSAE_CMD_LOG_OUTPUT]    = "log_output",

        [NSAE_CMD_FD_EJECT]      = "fd_eject",
        [NSAE_CMD_FD_LOAD]       = "fd_load",
        [NSAE_CMD_FD_SAVE]       = "fd_save",
        [NSAE_CMD_FD_BLK_READ]   = "fd_blkread",
        [NSAE_CMD_FD_STATUS]     = "fd_status",

        [NSAE_CMD_HD_EJECT]      = "hd_eject",
        [NSAE_CMD_HD_LOAD]       = "hd_load",
        [NSAE_CMD_HD_SAVE]       = "hd_save",
        [NSAE_CMD_HD_STATUS]     = "hd_status",

        [NSAE_CMD_KB_PUSH]       = "kb_push",
        [NSAE_CMD_KB_POP]        = "kb_pop",
        [NSAE_CMD_KB_OVERFLOW]   = "kb_overflow",
        [NSAE_CMD_KB_CAPS]       = "kb_caps",
        [NSAE_CMD_KB_CURSOR]     = "kb_cursor",
        [NSAE_CMD_KB_DATA]       = "kb_data",
        [NSAE_CMD_KB_INTERUPT]   = "kb_interupt",
        [NSAE_CMD_KB_STATUS]     = "kb_status",

        [NSAE_CMD_IO_STATUS]     = "io_status",

        [NSAE_CMD_CRT_STATUS]    = "crt_status",

        [NSAE_CMD_ADV_STATUS]    = "adv_status",
        [NSAE_CMD_ADV_OUT]       = "adv_out",
        [NSAE_CMD_ADV_IN]        = "adv_in",

        [NSAE_CMD_CPU_STATUS]    = "cpu_status",

        [NSAE_CMD_RAM_READ]      = "ram_read",
        [NSAE_CMD_RAM_WRITE]     = "ram_write",

        [NSAE_CMD_PROM_LOAD]     = "prom_load",

        [NSAE_CMD_MMU_READ]      = "mmu_read",
        [NSAE_CMD_MMU_WRITE]     = "mmu_write",
        [NSAE_CMD_MMU_LOAD]      = "mmu_load",
        [NSAE_CMD_MMU_STATUS]    = "mmu_status",
    };

    argc -= optind;
    argv += optind;

    if (argc <= 0)
    {
        log_error ("nsaectl: missing field -- mode\n");
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
        log_error ("nsaectl: invalid mode -- \"%s\"\n", mode_str);
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    int mode_argc[NSAE_CMD_COUNT] = {
        [NSAE_CMD_EXIT]          = 1,
        [NSAE_CMD_RESTART]       = 1,
        [NSAE_CMD_PAUSE]         = 1,
        [NSAE_CMD_CONTINUE]      = 1,
        [NSAE_CMD_BRKPNT_SET]    = 2,
        [NSAE_CMD_BRKPNT_REMOVE] = 2,
        [NSAE_CMD_BRKPNT_LIST]   = 1,
        [NSAE_CMD_STEP]          = 1,
        [NSAE_CMD_RUN]           = 1,
        [NSAE_CMD_STATUS]        = 1,

        [NSAE_CMD_LOG_CPU]       = 2,
        [NSAE_CMD_LOG_MMU]       = 2,
        [NSAE_CMD_LOG_RAM]       = 2,
        [NSAE_CMD_LOG_FDC]       = 2,
        [NSAE_CMD_LOG_CRT]       = 2,
        [NSAE_CMD_LOG_KB]        = 2,
        [NSAE_CMD_LOG_MOBO]      = 2,
        [NSAE_CMD_LOG_TERSE]     = 1,
        [NSAE_CMD_LOG_VERBOSE]   = 1,
        [NSAE_CMD_LOG_DEBUG]     = 1,
        [NSAE_CMD_LOG_OUTPUT]    = 2,

        [NSAE_CMD_FD_EJECT]      = 2,
        [NSAE_CMD_FD_LOAD]       = 3,
        [NSAE_CMD_FD_SAVE]       = 3,
        [NSAE_CMD_FD_BLK_READ]   = 5,
        [NSAE_CMD_FD_STATUS]     = 1,

        [NSAE_CMD_HD_EJECT]      = 1,
        [NSAE_CMD_HD_LOAD]       = 2,
        [NSAE_CMD_HD_SAVE]       = 2,
        [NSAE_CMD_HD_STATUS]     = 1,

        [NSAE_CMD_KB_PUSH]       = 2,
        [NSAE_CMD_KB_POP]        = 1,
        [NSAE_CMD_KB_OVERFLOW]   = 2,
        [NSAE_CMD_KB_CAPS]       = 2,
        [NSAE_CMD_KB_CURSOR]     = 2,
        [NSAE_CMD_KB_DATA]       = 2,
        [NSAE_CMD_KB_INTERUPT]   = 2,
        [NSAE_CMD_KB_STATUS]     = 1,

        [NSAE_CMD_IO_STATUS]     = 1,

        [NSAE_CMD_CRT_STATUS]    = 1,

        [NSAE_CMD_ADV_STATUS]    = 1,
        [NSAE_CMD_ADV_OUT]       = 3,
        [NSAE_CMD_ADV_IN]        = 2,

        [NSAE_CMD_CPU_STATUS]    = 1,

        [NSAE_CMD_RAM_READ]      = 3,
        [NSAE_CMD_RAM_WRITE]     = 3,

        [NSAE_CMD_PROM_LOAD]     = 2,

        [NSAE_CMD_MMU_READ]      = 3,
        [NSAE_CMD_MMU_WRITE]     = 3,
        [NSAE_CMD_MMU_LOAD]      = 3,
        [NSAE_CMD_MMU_STATUS]    = 1,
    };

    if (argc != mode_argc[mode])
    {
        log_error ("nsaectl: missing mode argument\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    log_verbose ("nsaectl: openning connection to server\n");
    int rc = nsae_ipc_init (NSAE_IPC_CLIENT, custom_client, custom_server);
    if (rc != 0)
    {
        log_error ("nsaectl: failed to initialize ipc\n");
        exit (EXIT_FAILURE);
    }
    log_verbose ("nsaectl: connection opened\n");


    log_verbose ("nsaectl: parsing mode\n");
    switch (mode)
    {
    /* u = (u8|u16|u32); */

    /* cmd u */
    case NSAE_CMD_BRKPNT_SET:   /* cmd addr */
    case NSAE_CMD_BRKPNT_REMOVE:/* cmd addr */
    case NSAE_CMD_LOG_CPU:      /* cmd state */
    case NSAE_CMD_LOG_MMU:      /* cmd state */
    case NSAE_CMD_LOG_RAM:      /* cmd state */
    case NSAE_CMD_LOG_FDC:      /* cmd state */
    case NSAE_CMD_LOG_CRT:      /* cmd state */
    case NSAE_CMD_LOG_KB:       /* cmd state */
    case NSAE_CMD_LOG_MOBO:     /* cmd state */
    case NSAE_CMD_FD_EJECT:     /* cmd fd_num */
    case NSAE_CMD_KB_PUSH:      /* cmd keycode */
    case NSAE_CMD_KB_OVERFLOW:  /* cmd state */
    case NSAE_CMD_KB_CAPS:      /* cmd state */
    case NSAE_CMD_KB_CURSOR:    /* cmd state */
    case NSAE_CMD_KB_DATA:      /* cmd state */
    case NSAE_CMD_KB_INTERUPT:  /* cmd state */
    case NSAE_CMD_ADV_IN:       /* cmd port */
        send_cmd (mode, strtol (argv[1], NULL, 0));
        break;

    /* cmd u u */
    case NSAE_CMD_ADV_OUT:      /* cmd port data */
    case NSAE_CMD_RAM_READ:     /* cmd abs_addr len */
    case NSAE_CMD_RAM_WRITE:    /* cmd abs_addr data */
    case NSAE_CMD_MMU_READ:     /* cmd addr len */
    case NSAE_CMD_MMU_WRITE:    /* cmd addr data */
    case NSAE_CMD_MMU_LOAD:     /* cmd slot page */
        send_cmd (mode, strtol (argv[1], NULL, 0), strtol (argv[2], NULL, 0));
        break;

    /* cmd u u u u */
    case NSAE_CMD_FD_BLK_READ:  /* cmd fd_num side track sec */
        send_cmd (mode,
                strtol (argv[1], NULL, 0),
                strtol (argv[2], NULL, 0),
                strtol (argv[3], NULL, 0),
                strtol (argv[4], NULL, 0));
        break;

    /* cmd u file */
    case NSAE_CMD_FD_LOAD:
    case NSAE_CMD_FD_SAVE:
        send_cmd (mode, strtol (argv[1], NULL, 0), argv[2]);
        break;

    /* cmd file */
    case NSAE_CMD_LOG_OUTPUT:
    case NSAE_CMD_HD_LOAD:
    case NSAE_CMD_HD_SAVE:
    case NSAE_CMD_PROM_LOAD:
        send_cmd (mode, argv[1]);
        break;

    /* cmd */
    case NSAE_CMD_EXIT:
    case NSAE_CMD_RESTART:
    case NSAE_CMD_PAUSE:
    case NSAE_CMD_CONTINUE:
    case NSAE_CMD_BRKPNT_LIST:
    case NSAE_CMD_STEP:
    case NSAE_CMD_RUN:
    case NSAE_CMD_STATUS:
    case NSAE_CMD_LOG_TERSE:
    case NSAE_CMD_LOG_VERBOSE:
    case NSAE_CMD_LOG_DEBUG:
    case NSAE_CMD_FD_STATUS:
    case NSAE_CMD_HD_EJECT:
    case NSAE_CMD_KB_POP:
    case NSAE_CMD_KB_STATUS:
    case NSAE_CMD_IO_STATUS:
    case NSAE_CMD_CRT_STATUS:
    case NSAE_CMD_ADV_STATUS:
    case NSAE_CMD_CPU_STATUS:
    case NSAE_CMD_MMU_STATUS:
        send_cmd (mode);
        break;

    /* error */
    default: 
        abort ();
    }

    log_verbose ("nsaectl: closing connection to server\n");
    nsae_ipc_free (NSAE_IPC_CLIENT);

    log_quit ();

    return 0;
}


/* end of file */
