
#define LOG_CATEGORY LC_GENERAL
//#include "client.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "nsaectl_help.h"
#include "nsaectl_version.h"
#include "nslog.h"

#include <stb_ds.h>

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

#define ARRLEN(a) (sizeof (a) / sizeof (*(a)))

typedef struct {
    uint8_t cmd;
    int min_args;
    int max_args;
} concmd_t;

typedef struct {
    char *key;
    concmd_t value;
} concmd_hash_t;


concmd_hash_t *
get_keywords (void)
{
    concmd_hash_t *iter = NULL;
    concmd_hash_t *hash = NULL;

    concmd_hash_t cmd_arr[] = {
        { .key = "exit",          .value = { NSAE_CMD_EXIT,          0, 0 } },
        { .key = "restart",       .value = { NSAE_CMD_RESTART,       0, 0 } },
        { .key = "pause",         .value = { NSAE_CMD_PAUSE,         0, 0 } },
        { .key = "continue",      .value = { NSAE_CMD_CONTINUE,      0, 0 } },
        { .key = "brkpnt_set",    .value = { NSAE_CMD_BRKPNT_SET,    1, 1 } },
        { .key = "brkpnt_remove", .value = { NSAE_CMD_BRKPNT_REMOVE, 1, 1 } },
        { .key = "brkpnt_list",   .value = { NSAE_CMD_BRKPNT_LIST,   0, 0 } },
        { .key = "step",          .value = { NSAE_CMD_STEP,          0, 0 } },
        { .key = "run",           .value = { NSAE_CMD_RUN,           0, 0 } },
        { .key = "status",        .value = { NSAE_CMD_STATUS,        0, 0 } },
        { .key = "log_cpu",       .value = { NSAE_CMD_LOG_CPU,       1, 1 } },
        { .key = "log_mmu",       .value = { NSAE_CMD_LOG_MMU,       1, 1 } },
        { .key = "log_ram",       .value = { NSAE_CMD_LOG_RAM,       1, 1 } },
        { .key = "log_fdc",       .value = { NSAE_CMD_LOG_FDC,       1, 1 } },
        { .key = "log_crt",       .value = { NSAE_CMD_LOG_CRT,       1, 1 } },
        { .key = "log_kb",        .value = { NSAE_CMD_LOG_KB,        1, 1 } },
        { .key = "log_mobo",      .value = { NSAE_CMD_LOG_MOBO,      1, 1 } },
        { .key = "log_terse",     .value = { NSAE_CMD_LOG_TERSE,     0, 0 } },
        { .key = "log_verbose",   .value = { NSAE_CMD_LOG_VERBOSE,   0, 0 } },
        { .key = "log_debug",     .value = { NSAE_CMD_LOG_DEBUG,     0, 0 } },
        { .key = "log_output",    .value = { NSAE_CMD_LOG_OUTPUT,    1, 1 } },
        { .key = "fd_eject",      .value = { NSAE_CMD_FD_EJECT,      1, 1 } },
        { .key = "fd_load",       .value = { NSAE_CMD_FD_LOAD,       2, 2 } },
        { .key = "fd_save",       .value = { NSAE_CMD_FD_SAVE,       2, 2 } },
        { .key = "fd_blk_read",   .value = { NSAE_CMD_FD_BLK_READ,   4, 4 } },
        { .key = "fd_status",     .value = { NSAE_CMD_FD_STATUS,     0, 0 } },
        { .key = "hd_eject",      .value = { NSAE_CMD_HD_EJECT,      0, 0 } },
        { .key = "hd_load",       .value = { NSAE_CMD_HD_LOAD,       1, 1 } },
        { .key = "hd_save",       .value = { NSAE_CMD_HD_SAVE,       1, 1 } },
        { .key = "hd_status",     .value = { NSAE_CMD_HD_STATUS,     0, 0 } },
        { .key = "kb_push",       .value = { NSAE_CMD_KB_PUSH,       1, 1 } },
        { .key = "kb_pop",        .value = { NSAE_CMD_KB_POP,        0, 0 } },
        { .key = "kb_overflow",   .value = { NSAE_CMD_KB_OVERFLOW,   1, 1 } },
        { .key = "kb_caps",       .value = { NSAE_CMD_KB_CAPS,       1, 1 } },
        { .key = "kb_cursor",     .value = { NSAE_CMD_KB_CURSOR,     1, 1 } },
        { .key = "kb_data",       .value = { NSAE_CMD_KB_DATA,       1, 1 } },
        { .key = "kb_interupt",   .value = { NSAE_CMD_KB_INTERUPT,   1, 1 } },
        { .key = "kb_status",     .value = { NSAE_CMD_KB_STATUS,     0, 0 } },
        { .key = "io_status",     .value = { NSAE_CMD_IO_STATUS,     0, 0 } },
        { .key = "crt_status",    .value = { NSAE_CMD_CRT_STATUS,    0, 0 } },
        { .key = "adv_out",       .value = { NSAE_CMD_ADV_OUT,       2, 2 } },
        { .key = "adv_in",        .value = { NSAE_CMD_ADV_IN,        1, 1 } },
        { .key = "adv_status",    .value = { NSAE_CMD_ADV_STATUS,    0, 0 } },
        { .key = "cpu_status",    .value = { NSAE_CMD_CPU_STATUS,    0, 0 } },
        { .key = "ram_read",      .value = { NSAE_CMD_RAM_READ,      2, 2 } },
        { .key = "ram_write",     .value = { NSAE_CMD_RAM_WRITE,     2, 2 } },
        { .key = "prom_load",     .value = { NSAE_CMD_PROM_LOAD,     1, 1 } },
        { .key = "mmu_read",      .value = { NSAE_CMD_MMU_READ,      2, 2 } },
        { .key = "mmu_write",     .value = { NSAE_CMD_MMU_WRITE,     2, 2 } },
        { .key = "mmu_load",      .value = { NSAE_CMD_MMU_LOAD,      2, 2 } },
        { .key = "mmu_status",    .value = { NSAE_CMD_MMU_STATUS,    0, 0 } },
    };
    concmd_t default_cmd = (concmd_t){ NSAE_CMD_COUNT, 0, 0 };

    shdefault (hash, default_cmd);

    for (size_t i = 0; i < ARRLEN (cmd_arr); i++)
    {
        iter = &cmd_arr[i];
        shput (hash, iter->key, iter->value);
    }

    return hash;
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
    argc -= optind;
    argv += optind;

    /* mode selection */
    if (argc <= 0)
    {
        log_error ("nsaectl: missing field -- mode\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    const char *mode_str = argv[0];

    concmd_hash_t *mode_hash = get_keywords ();
    concmd_t mode = shget (mode_hash, mode_str);
    argc--; argv++;

     assert (mode.cmd <= NSAE_CMD_COUNT);
     if (mode.cmd == NSAE_CMD_COUNT)
     {
         log_error ("nsaectl: invalid mode -- \"%s\"\n", mode_str);
         fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
         exit (EXIT_FAILURE);
    }

    if ((mode.min_args > argc) || (argc > mode.max_args))
    {
        log_error ("nsaectl: missing mode argument\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    log_verbose ("nsaectl: building packet\n");
    size_t packet_size = sizeof (nsae_packet_t);
    nsae_packet_t *packet = malloc (packet_size);

    packet->cmd = mode.cmd;

    switch (packet->cmd)
    {
    case NSAE_CMD_BRKPNT_SET:
    case NSAE_CMD_BRKPNT_REMOVE:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_LOG_CPU:
    case NSAE_CMD_LOG_MMU:
    case NSAE_CMD_LOG_RAM:
    case NSAE_CMD_LOG_FDC:
    case NSAE_CMD_LOG_CRT:
    case NSAE_CMD_LOG_KB:
    case NSAE_CMD_LOG_MOBO:
    case NSAE_CMD_KB_OVERFLOW:
    case NSAE_CMD_KB_CAPS:
    case NSAE_CMD_KB_CURSOR:
    case NSAE_CMD_KB_DATA:
    case NSAE_CMD_KB_INTERUPT:
        packet->v_state = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_LOG_OUTPUT:
    case NSAE_CMD_HD_LOAD:
    case NSAE_CMD_HD_SAVE:
    case NSAE_CMD_PROM_LOAD:
        packet->v_buflen = strlen (argv[0]);
        packet_size += packet->v_buflen;
        packet = realloc (packet, packet_size);
        memcpy (&packet->buf, argv[0], packet->v_buflen);
        break;
    
    case NSAE_CMD_FD_EJECT:
        packet->v_fd_num = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_FD_LOAD:
    case NSAE_CMD_FD_SAVE:
        packet->v_fd_num = strtol (argv[0], NULL, 0);
        packet->v_buflen = strlen (argv[1]);
        packet_size += packet->v_buflen;
        packet = realloc (packet, packet_size);
        memcpy (&packet->buf, argv[1], packet->v_buflen);
        break;

    case NSAE_CMD_FD_BLK_READ:
        packet->v_fd_num    = strtol (argv[0], NULL, 0);
        packet->v_fd_side   = strtol (argv[1], NULL, 0);
        packet->v_fd_track  = strtol (argv[2], NULL, 0);
        packet->v_fd_sector = strtol (argv[3], NULL, 0);
        break;

    case NSAE_CMD_KB_PUSH:
        packet->v_keycode = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_ADV_OUT:
        packet->v_port = strtol (argv[0], NULL, 0);
        packet->v_data = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_ADV_IN:
        packet->v_port = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_RAM_READ:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        packet->v_span32 = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_RAM_WRITE:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        packet->v_data   = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_MMU_READ:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        packet->v_span16 = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_MMU_WRITE:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        packet->v_data   = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_MMU_LOAD:
        packet->v_slot = strtol (argv[0], NULL, 0);
        packet->v_page = strtol (argv[1], NULL, 0);
        break;

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
    case NSAE_CMD_HD_EJECT:
    case NSAE_CMD_KB_POP:
    case NSAE_CMD_KB_STATUS:
    case NSAE_CMD_IO_STATUS:
    case NSAE_CMD_CRT_STATUS:
    case NSAE_CMD_ADV_STATUS:
    case NSAE_CMD_CPU_STATUS:
    case NSAE_CMD_MMU_STATUS:
        break;

    default:
        log_warning ("nsaectl: error: unknown cmd -- %x\n", packet->cmd);
        break;
    }

    log_verbose ("nsaectl: openning connection to server\n");
    int rc = nsae_ipc_init (NSAE_IPC_CLIENT, custom_client, custom_server);
    if (rc != 0)
    {
        log_fatal ("nsaectl: failed to initialize ipc\n");
        exit (EXIT_FAILURE);
    }

    log_verbose ("nsaectl: sending packet\n");
    (void)nsae_ipc_send_block (&packet_size, sizeof (uint32_t));
    (void)nsae_ipc_send_block (packet, packet_size);

    log_verbose ("nsaectl: closing connection to server\n");
    nsae_ipc_free (NSAE_IPC_CLIENT);

    log_quit ();

    return 0;
}


/* end of file */
