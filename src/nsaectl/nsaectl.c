
#define LOG_CATEGORY LC_GENERAL
#include "nsaecmd.h"
#include "nsaectl_help.h"
#include "nsaectl_version.h"
#include "nsaeipc.h"
#include "nslog.h"

#include <sc_map.h>

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

struct sc_map_sv
get_keywords (void)
{
    struct sc_map_sv map;
    static struct { const char *key; concmd_t value; } s_cmd_arr[] = {
        { "adv_in",      { NSAE_CMD_ADV_IN,        1, 1 } },
        { "adv_out",     { NSAE_CMD_ADV_OUT,       2, 2 } },
        { "adv_status",  { NSAE_CMD_ADV_STATUS,    0, 0 } },
        { "br_list",     { NSAE_CMD_BRKPNT_LIST,   0, 0 } },
        { "br_remove",   { NSAE_CMD_BRKPNT_REMOVE, 1, 1 } },
        { "br_set",      { NSAE_CMD_BRKPNT_SET,    1, 1 } },
        { "continue",    { NSAE_CMD_CONTINUE,      0, 0 } },
        { "cpu_status",  { NSAE_CMD_CPU_STATUS,    0, 0 } },
        { "crt_status",  { NSAE_CMD_CRT_STATUS,    0, 0 } },
        { "exit",        { NSAE_CMD_EXIT,          0, 0 } },
        { "fd_blk_read", { NSAE_CMD_FD_BLK_READ,   4, 4 } },
        { "fd_eject",    { NSAE_CMD_FD_EJECT,      1, 1 } },
        { "fd_load",     { NSAE_CMD_FD_LOAD,       2, 2 } },
        { "fd_save",     { NSAE_CMD_FD_SAVE,       2, 2 } },
        { "fd_status",   { NSAE_CMD_FD_STATUS,     0, 0 } },
        { "hd_eject",    { NSAE_CMD_HD_EJECT,      0, 0 } },
        { "hd_load",     { NSAE_CMD_HD_LOAD,       1, 1 } },
        { "hd_save",     { NSAE_CMD_HD_SAVE,       1, 1 } },
        { "hd_status",   { NSAE_CMD_HD_STATUS,     0, 0 } },
        { "io_status",   { NSAE_CMD_IO_STATUS,     0, 0 } },
        { "kb_caps",     { NSAE_CMD_KB_CAPS,       1, 1 } },
        { "kb_cursor",   { NSAE_CMD_KB_CURSOR,     1, 1 } },
        { "kb_data",     { NSAE_CMD_KB_DATA,       1, 1 } },
        { "kb_interupt", { NSAE_CMD_KB_INTERUPT,   1, 1 } },
        { "kb_overflow", { NSAE_CMD_KB_OVERFLOW,   1, 1 } },
        { "kb_pop",      { NSAE_CMD_KB_POP,        0, 0 } },
        { "kb_push",     { NSAE_CMD_KB_PUSH,       1, 1 } },
        { "kb_status",   { NSAE_CMD_KB_STATUS,     0, 0 } },
        { "log_cpu",     { NSAE_CMD_LOG_CPU,       1, 1 } },
        { "log_crt",     { NSAE_CMD_LOG_CRT,       1, 1 } },
        { "log_debug",   { NSAE_CMD_LOG_DEBUG,     0, 0 } },
        { "log_fdc",     { NSAE_CMD_LOG_FDC,       1, 1 } },
        { "log_kb",      { NSAE_CMD_LOG_KB,        1, 1 } },
        { "log_mmu",     { NSAE_CMD_LOG_MMU,       1, 1 } },
        { "log_mobo",    { NSAE_CMD_LOG_MOBO,      1, 1 } },
        { "log_output",  { NSAE_CMD_LOG_OUTPUT,    1, 1 } },
        { "log_ram",     { NSAE_CMD_LOG_RAM,       1, 1 } },
        { "log_terse",   { NSAE_CMD_LOG_TERSE,     0, 0 } },
        { "log_verbose", { NSAE_CMD_LOG_VERBOSE,   0, 0 } },
        { "mmu_load",    { NSAE_CMD_MMU_LOAD,      2, 2 } },
        { "mmu_read",    { NSAE_CMD_MMU_READ,      1, 2 } },
        { "mmu_status",  { NSAE_CMD_MMU_STATUS,    0, 0 } },
        { "mmu_write",   { NSAE_CMD_MMU_WRITE,     2, 2 } },
        { "pause",       { NSAE_CMD_PAUSE,         0, 0 } },
        { "prom_load",   { NSAE_CMD_PROM_LOAD,     1, 1 } },
        { "ram_read",    { NSAE_CMD_RAM_READ,      1, 2 } },
        { "ram_write",   { NSAE_CMD_RAM_WRITE,     2, 2 } },
        { "restart",     { NSAE_CMD_RESTART,       0, 0 } },
        { "run",         { NSAE_CMD_RUN,           0, 0 } },
        { "status",      { NSAE_CMD_STATUS,        0, 0 } },
        { "step",        { NSAE_CMD_STEP,          0, 0 } },
        { "wp_list",     { NSAE_CMD_WP_LIST,       0, 0 } },
        { "wp_remove",   { NSAE_CMD_WP_REMOVE,     1, 1 } },
        { "wp_set",      { NSAE_CMD_WP_SET,        1, 4 } },
    };

    sc_map_init_sv (&map, 0, 0);

    for (size_t i = 0; i < ARRLEN (s_cmd_arr); i++)
    {
	sc_map_put_sv (&map, s_cmd_arr[i].key, (void *)&s_cmd_arr[i].value);
    }

    return map;
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

    struct sc_map_sv mode_hash = get_keywords ();
    concmd_t *p_mode = NULL;
    bool match = sc_map_get_sv (&mode_hash, mode_str, (void **)&p_mode);
    argc--; argv++;

    if (!match)
    {
         log_error ("nsaectl: invalid mode -- \"%s\"\n", mode_str);
         fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
         exit (EXIT_FAILURE);
    }
    assert (p_mode != NULL);
    assert (p_mode->cmd <= NSAE_CMD_COUNT);

    if ((p_mode->min_args > argc) || (argc > p_mode->max_args))
    {
        log_error ("nsaectl: missing mode argument\n");
        fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
        exit (EXIT_FAILURE);
    }

    log_verbose ("nsaectl: building packet\n");
    size_t packet_size = sizeof (nsae_packet_t);
    nsae_packet_t *packet = malloc (packet_size);

    packet->cmd = p_mode->cmd;

    switch (packet->cmd)
    {
    case NSAE_CMD_BRKPNT_SET:
    case NSAE_CMD_BRKPNT_REMOVE:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        break;

    case NSAE_CMD_WP_SET:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        if (argc <= 1)
        {
            /* if no data is given, match any updates to that address */
            packet->v_data16= 0;
            packet->v_wp_type = 0;
            packet->v_wp_match = 0;
        }
        else
        {
            /* otherwise default to matching when equal to data */
            packet->v_data16   = strtol (argv[1], NULL, 0);
            packet->v_wp_match = (argc <= 2) ? 1 : strtol (argv[2], NULL, 0);
            packet->v_wp_type  = (argc <= 3) ? 0 : strtol (argv[3], NULL, 0);
        }
        break;

    case NSAE_CMD_WP_REMOVE:
        packet->v_data = strtol (argv[0], NULL, 0);
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
        packet->v_span32 = (argc < 1) ? 1 : strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_RAM_WRITE:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        packet->v_data   = strtol (argv[1], NULL, 0);
        break;

    case NSAE_CMD_MMU_READ:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        packet->v_span16 = (argc < 1) ? 1 : strtol (argv[1], NULL, 0);
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
    case NSAE_CMD_WP_LIST:
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
