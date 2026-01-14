
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

#define ARG_POP() (assert (argc >= 0), argc--, *argv++)

#define ARRLEN(a) (sizeof (a) / sizeof (*(a)))


enum {
    NSAE_IMPLICIT_RESET = NSAE_CMD_EXTRA,
    NSAE_IMPLICIT_BREAK,
    NSAE_IMPLICIT_SEND,
};

static const struct { 
    const char *cmd; 
    const char *modes; 
    uint8_t def_mode; 
    uint8_t cmd_id; 
} CMD_LIST[] = {
    /* {{{ */
    { "quit",     NULL, NULL, NSAE_CMD_QUIT },
    { "run",      NULL, NULL, NSAE_CMD_RUN  },
    { "step",     NULL, NULL, NSAE_CMD_STEP },
    { "next",     NULL, NULL, NSAE_CMD_NEXT },
    { "continue", NULL, NULL, NSAE_CMD_CONTINUE },
    { "reset",    NULL, NULL, NSAE_IMPLICIT_RESET },

    { "info",     "nsae:breakpoint:advantage:cpu:display:floppy:harddrive"
                  ":keyboard:mmu:ram:log", NULL, NSAE_CMD_INFO },

    { "set",      "nsae:breakpoint:advantage:port:cpu:display:floppy"
                  ":harddrive:keyboard:mmu:log", NULL, NSAE_CMD_SET },

    { "delete",   "breakpoint:key", NSAE_MODE_KEYBOARD, NSAE_CMD_DELETE },
    { "load",     "floppy:harddrive:prom:mmu:ram", NSAE_MODE_FLOPPY, NSAE_CMD_LOAD },
    { "save",     "floppy:harddrive:prom:mmu:ram", NSAE_MODE_FLOPPY, NSAE_CMD_SAVE },
    { "read",     "port:floppy:harddrive:mmu:ram", NSAE_MODE_MMU, NSAE_CMD_READ },
    { "write",    "mmu:ram", NSAE_MODE_MMU, NSAE_CMD_WRITE },

    { "break", NULL, NULL, NSAE_IMPLICIT_BREAK },
    { "send",  NULL, NULL, NSAE_IMPLICIT_SEND },
    /* }}} */
};

static const struct { 
    const char *name; 
    uint8_t mode_id; 
} MODE_LIST[] = {
    /* {{{ */
    { "nsae",       NSAE_MODE_NSAE },
    { "breakpoint", NSAE_MODE_BREAKPOINT },
    { "advantage",  NSAE_MODE_ADVANTAGE },
    { "cpu",        NSAE_MODE_CPU },
    { "display",    NSAE_MODE_DISPLAY },
    { "floppy",     NSAE_MODE_FLOPPY },
    { "harddrive",  NSAE_MODE_HARDDRIVE },
    { "prom",       NSAE_MODE_PROM },
    { "keyboard",   NSAE_MODE_KEYBOARD },
    { "mmu",        NSAE_MODE_MMU },
    { "ram",        NSAE_MODE_RAM },
    { "log",        NSAE_MODE_LOG },
    { "all",        NSAE_MODE_ALL },
    /* }}} */
};

static const struct { 
    uint8_t mode_id; 
    const char *name; 
    uint8_t set_id; 
} SET_LIST[] = {
    /* {{{ */
    { NSAE_MODE_BREAKPOINT, "address", NSAE_VAR_BREAKPOINT_APPEND },

    { NSAE_MODE_ADVANTAGE, "kbmi",     NSAE_VAR_ADVANTAGE_KBMI },
    { NSAE_MODE_ADVANTAGE, "kbnmi",    NSAE_VAR_ADVANTAGE_KBNMI },
    { NSAE_MODE_ADVANTAGE, "crtmi",    NSAE_VAR_ADVANTAGE_CRTMI },
    { NSAE_MODE_ADVANTAGE, "interupt", NSAE_VAR_ADVANTAGE_INTERUPT },
    { NSAE_MODE_ADVANTAGE, "cmdack",   NSAE_VAR_ADVANTAGE_CMDACK },

    // { NSAE_MODE_PORT, "", NSAE_VAR_ADVANTAGE_CMDACK },

    { NSAE_MODE_CPU, "a",      NSAE_VAR_CPU_A },
    { NSAE_MODE_CPU, "bc",     NSAE_VAR_CPU_BC },
    { NSAE_MODE_CPU, "de",     NSAE_VAR_CPU_DE },
    { NSAE_MODE_CPU, "hl",     NSAE_VAR_CPU_HL },
    { NSAE_MODE_CPU, "pc",     NSAE_VAR_CPU_PC },
    { NSAE_MODE_CPU, "sp",     NSAE_VAR_CPU_SP },
    { NSAE_MODE_CPU, "ix",     NSAE_VAR_CPU_IX },
    { NSAE_MODE_CPU, "iy",     NSAE_VAR_CPU_IY },
    { NSAE_MODE_CPU, "i",      NSAE_VAR_CPU_I },
    { NSAE_MODE_CPU, "r",      NSAE_VAR_CPU_R },
    { NSAE_MODE_CPU, "iff1",   NSAE_VAR_CPU_IFF1 },
    { NSAE_MODE_CPU, "iff2",   NSAE_VAR_CPU_IFF2 },
    { NSAE_MODE_CPU, "im",     NSAE_VAR_CPU_IM },
    { NSAE_MODE_CPU, "exx",    NSAE_VAR_CPU_EXX },
    { NSAE_MODE_CPU, "halt",   NSAE_VAR_CPU_HALT },
    { NSAE_MODE_CPU, "s_flag", NSAE_VAR_CPU_S_FLAG },
    { NSAE_MODE_CPU, "z_flag", NSAE_VAR_CPU_Z_FLAG },
    { NSAE_MODE_CPU, "h_flag", NSAE_VAR_CPU_H_FLAG },
    { NSAE_MODE_CPU, "p_flag", NSAE_VAR_CPU_P_FLAG },
    { NSAE_MODE_CPU, "v_flag", NSAE_VAR_CPU_V_FLAG },
    { NSAE_MODE_CPU, "n_flag", NSAE_VAR_CPU_N_FLAG },
    { NSAE_MODE_CPU, "c_flag", NSAE_VAR_CPU_C_FLAG },

    { NSAE_MODE_DISPLAY, "color",  NSAE_VAR_DISPLAY_COLOR },
    { NSAE_MODE_DISPLAY, "colour", NSAE_VAR_DISPLAY_COLOR },
    { NSAE_MODE_DISPLAY, "blank",  NSAE_VAR_DISPLAY_BLANK },
    { NSAE_MODE_DISPLAY, "vsync",  NSAE_VAR_DISPLAY_VSYNC },
    { NSAE_MODE_DISPLAY, "scroll",  NSAE_VAR_DISPLAY_SCROLL },

    { NSAE_MODE_FLOPPY, "disk",       NSAE_VAR_FLOPPY_DISK },
    { NSAE_MODE_FLOPPY, "side",       NSAE_VAR_FLOPPY_SIDE },
    { NSAE_MODE_FLOPPY, "track",      NSAE_VAR_FLOPPY_TRACK },
    { NSAE_MODE_FLOPPY, "powered",    NSAE_VAR_FLOPPY_POWERED },
    { NSAE_MODE_FLOPPY, "trackzero",  NSAE_VAR_FLOPPY_TRACKZERO },
    { NSAE_MODE_FLOPPY, "sectormark", NSAE_VAR_FLOPPY_SECTORMARK },
    { NSAE_MODE_FLOPPY, "0:eject",    NSAE_VAR_FLOPPY_EJECT_A },
    { NSAE_MODE_FLOPPY, "1:eject",    NSAE_VAR_FLOPPY_EJECT_B },
    { NSAE_MODE_FLOPPY, "0:sector",   NSAE_VAR_FLOPPY_SECTOR_A },
    { NSAE_MODE_FLOPPY, "1:sector",   NSAE_VAR_FLOPPY_SECTOR_B },

    { NSAE_MODE_HARDDRIVE, "eject", NSAE_VAR_HARDDRIVE_EJECT },

    { NSAE_MODE_KEYBOARD, "repeat",     NSAE_VAR_KEYBOARD_REPEAT },
    { NSAE_MODE_KEYBOARD, "capslock",   NSAE_VAR_KEYBOARD_CAPSLOCK },
    { NSAE_MODE_KEYBOARD, "cursorlock", NSAE_VAR_KEYBOARD_CURSORLOCK },
    { NSAE_MODE_KEYBOARD, "overflow",   NSAE_VAR_KEYBOARD_OVERFLOW },
    { NSAE_MODE_KEYBOARD, "dataflag",   NSAE_VAR_KEYBOARD_DATAFLAG },
    { NSAE_MODE_KEYBOARD, "interrupt",  NSAE_VAR_KEYBOARD_INTERRUPT },
    { NSAE_MODE_KEYBOARD, "press",      NSAE_VAR_KEYBOARD_PRESS },

    { NSAE_MODE_MMU, "slot0", NSAE_VAR_MMU_SLOT0 },
    { NSAE_MODE_MMU, "slot1", NSAE_VAR_MMU_SLOT1 },
    { NSAE_MODE_MMU, "slot2", NSAE_VAR_MMU_SLOT2 },
    { NSAE_MODE_MMU, "slot3", NSAE_VAR_MMU_SLOT3 },

    { NSAE_MODE_LOG, "display",   NSAE_VAR_LOG_DISPLAY },
    { NSAE_MODE_LOG, "cpu",       NSAE_VAR_LOG_CPU },
    { NSAE_MODE_LOG, "keyboard",  NSAE_VAR_LOG_KEYBOARD },
    { NSAE_MODE_LOG, "ram",       NSAE_VAR_LOG_RAM },
    { NSAE_MODE_LOG, "mmu",       NSAE_VAR_LOG_MMU },
    { NSAE_MODE_LOG, "floppy",    NSAE_VAR_LOG_FLOPPY },
    { NSAE_MODE_LOG, "harddrive", NSAE_VAR_LOG_HARDDRIVE },
    { NSAE_MODE_LOG, "advantage", NSAE_VAR_LOG_ADVANTAGE },
    { NSAE_MODE_LOG, "all",       NSAE_VAR_LOG_ALL },
    { NSAE_MODE_LOG, "logfile",   NSAE_VAR_LOG_OUTPUT_FILE },
    /* }}} */
};


static int send (int argc, char **argv);

static void
version (void)
{
    fprintf (stderr, "%.*s", VERSION_LEN, VERSION_STR);
}

static void
usage (void)
{
    fprintf (stderr, "%.*s", USAGE_LEN, USAGE_STR);
}

int
main (int argc, char **argv)
{
    int rc = 1;
    int opt = 0;
    char *custom_server = NULL;
    char *custom_client = NULL;

    log_init (LC_COUNT);

    /* global options */
    while ((opt = getopt (argc, argv, "f:F:htvV")) != -1)
    {
        switch (opt)
        {
        case 'F': custom_client = optarg; break;
        case 'f': custom_server = optarg; break;
        case 't': log_set (LOG_INFO); break;
        case 'v': log_set (LOG_VERBOSE); break;

        case 'V': version (); exit (EXIT_SUCCESS); break;
        case 'h': usage ();   exit (EXIT_SUCCESS); break;
        default:  usage ();   exit (EXIT_FAILURE); break; /* error */
        }
    }
    argc -= optind;
    argv += optind;

    /* mode selection */
    if (argc <= 0)
    {
        log_error ("nsaectl: missing command\n");
        usage ();
        exit (EXIT_FAILURE);
    }

    /* setup ipc */
    nsae_ipc_init (NSAE_IPC_CLIENT, custom_client, custom_server);

    /* send command */
    if (send (argc, argv))
    {
        log_error ("nsaectl: failed to send\n");
        goto exit;
    }

    /* get result */

    rc = 0; /* successful exit */
exit:
    nsae_ipc_free (NSAE_IPC_CLIENT);
    return rc;
}

static uint8_t
get_cmd (const char *cmd_name)
{
    size_t i = 0;

    for (i = 0; i < ARRLEN (CMD_LIST); i++)
    {
        /* skip non-matching */
        if (0 != strcmp (cmd_name, CMD_LIST[i].cmd)) continue;

        return CMD_LIST[i].cmd_id;
    }

    return NSAE_CMD_NULL;
}

static int 
send (int argc, char **argv)
{
    int rc = 1;

    int i = 0;
    int j = 0;

    char *cmd_name = NULL;
    char *mode_name = NULL;
    nsae_packet_t packet = { 0 };
    nsae_packet_t *str_packet = NULL;

    memset (&packet, 0, sizeof (packet));

#if 0
    char *iter;
    char *tmp;
    char *file;
    size_t file_n;
    size_t new_size;
    size_t packet_size;
#endif


    /* check every command */
    cmd_name = ARG_POP ();
    packet.cmd = get_cmd (cmd_name);
    if (packet.cmd == NSAE_CMD_NULL)
    {
        log_fatal ("invalid command -- %s", cmd_name);
        return 1;
    }

    /* get mode if necessary */
    if (CMD_LIST[i].modes != NULL) 
    {
        /* check that mode is valid */
        if (argc > 0)
        {
            mode_name = *argv;
            assert (NULL == strchr (mode_name, ':'));

            for (j = 0; j < ARRLEN (MODE_LIST); j++)
            {
                if (0 != strcmp (mode_name, MODE_LIST[j].name)) continue;

                packet->mode = MODE_LIST[j].mode_id;
                break;
            }

            if (ARRLEN (MODE_LIST) == j)
            {
                /* invalid mode, use default */
                goto set_default_mode;
            }

            if (NULL == strstr (CMD_LIST[i].modes, mode_name))
            {
                /* invalid mode for command throw error */
                log_fatal ("command %s does not take any mode -- %s", 
                        cmd_name, mode_name);
                exit (EXIT_FAILURE);
            }

            ARG_POP ();
        }

set_default_mode:
        if (NSAE_MODE_NULL == packet->mode)
        {
            packet->mode = CMD_LIST[i].def_mode;
        }
    }

    /* command handling */
    switch (packet->cmd)
    {
    case NSAE_CMD_STEP:
        packet->v_count = (argc <= 0) ? 1 : strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_CMD_DELETE:
        packet->v_index = (argc <= 0) ? 1 : strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_CMD_QUIT:
    case NSAE_CMD_NEXT:
    case NSAE_CMD_CONTINUE:
    case NSAE_CMD_INFO:
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_CMD_RUN:
        if (argc > 0)
        {
            packet->cmd  = NSAE_CMD_SET;
            packet->mode = NSAE_MODE_CPU;
            packet->var  = NSAE_VAR_CPU_PC;
            packet->v_addr32 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (packet, sizeof (*packet));
        }

        packet->cmd = NSAE_CMD_RUN;
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_CMD_SET:
        tmp = ARG_POP ();
        for (i = 0; i < ARRLEN (SET_LIST); i++)
        {
            if (packet->mode !=  SET_LIST[i].mode_id) continue;
            if (0 != strcmp (tmp, SET_LIST[i].name)) continue;

            packet->var = SET_LIST[i].set_id;
            packet->v_data32 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (packet, sizeof (*packet));
            break;
        }

        if (i == ARRLEN (SET_LIST))
        {
            /* unknown variable name */
            log_fatal ("unknown variable name -- %s", tmp);
            exit (EXIT_FAILURE);
        }

        break;

    case NSAE_CMD_LOAD:
    case NSAE_CMD_SAVE:
        packet->v_addr32  = strtol (ARG_POP (), NULL, 0);
        packet->v_fddrive = strtol (ARG_POP (), NULL, 0);

        file = ARG_POP ();
        file_n = strlen (file);

        new_size = sizeof (*packet) + file_n + 1;
        tmp = realloc (packet, new_size);
        assert (tmp);
        packet = tmp;
        packet_size = new_size;

        memcpy (&packet->buf, file, file_n);
        nsae_ipc_send (packet, packet_size);
        break;

    case NSAE_CMD_READ:
        tmp = ARG_POP ();

        /* handle floppy format drive:sector */
        if (packet->mode == NSAE_MODE_FLOPPY)
        {
            packet->v_data8 = strtol (tmp, NULL, 0);

            iter = strchr (tmp, ':');
            if (iter == NULL)
            {
                /* missing drive select */
                log_fatal ("missing drive select");
                exit (EXIT_FAILURE);
            }

            packet->v_addr32 = strtol (iter + 1, NULL, 0);
        }
        else /* handle standard format */
        {
            packet->v_addr32 = strtol (tmp, NULL, 0);
        }

        packet->v_addr32 = (argc <= 0) ? 0x100 : strtol (ARG_POP (), NULL, 0);
        //format = (argc <= 0) ? "byte" : ARG_POP ();
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_CMD_WRITE:
        /* get address */
        packet->v_addr32 = strtol (ARG_POP (), NULL, 0);

        /* send data */
        do {
            packet->v_data8 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (packet, sizeof (*packet));

            packet->v_addr32++;
        } while (argc > 0);

        break;

    case NSAE_IMPLICIT_RESET:
        packet->cmd  = NSAE_CMD_SET;
        packet->mode = NSAE_MODE_MMU;
        packet->var  = NSAE_VAR_MMU_SLOT0;
        packet->v_data8 = 0x8;
        nsae_ipc_send (packet, sizeof (*packet));

        packet->var = NSAE_VAR_MMU_SLOT1;
        packet->v_data8 = 0x9;
        nsae_ipc_send (packet, sizeof (*packet));

        packet->var = NSAE_VAR_MMU_SLOT2;
        packet->v_data8 = 0xc;
        nsae_ipc_send (packet, sizeof (*packet));

        packet->var = NSAE_VAR_MMU_SLOT3;
        packet->v_data8 = 0x0;
        nsae_ipc_send (packet, sizeof (*packet));

        packet->cmd  = NSAE_CMD_SET;
        packet->mode = NSAE_MODE_CPU;
        packet->var  = NSAE_VAR_CPU_PC;
        packet->v_addr32 = 0x8000;
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_IMPLICIT_BREAK:
        packet->cmd    = NSAE_CMD_SET;
        packet->mode   = NSAE_MODE_BREAKPOINT;
        packet->var    = NSAE_VAR_BREAKPOINT_APPEND;
        packet->addr16 = strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (packet, sizeof (*packet));
        break;

    case NSAE_IMPLICIT_SEND:
        packet->cmd  = NSAE_CMD_SET;
        packet->mode = NSAE_MODE_KEYBOARD;
        packet->var  = NSAE_VAR_KEYBOARD_PRESS;

        /* get format and data */
        tmp = ARG_POP ();
        format = NULL;
        if (argc > 0)
        {
            format = ARG_POP ();
        }

        /* send data based on format */
        if ((format == NULL) || (0 == strcmp (format, "keycode"))
        {
            packet->data8 = strtol (tmp, NULL, 0);
            nsae_ipc_send (packet, sizeof (*packet));
        }
        else if (0 == strcmp (format, "string"))
        {
            for (; *tmp; tmp++)
            {
                packet->data8 = *tmp;
                nsae_ipc_send (packet, sizeof (*packet));
            }
        }
        else
        {
            /* error unknown format */
            log_fatal ("unknown format specifier -- %s", format);
            exit (EXIT_FAILURE);
        }
        break;

    default:
        /* unknown command */
        log_fatal ("unknown command id -- %d", packet->cmd);
        exit (EXIT_FAILURE);
    }

    return 0;

























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
        memcpy (packet->buf, argv[0], packet->v_buflen);
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
        memcpy (packet->buf, argv[1], packet->v_buflen);
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
    (void)nsae_ipc_send_block (packet_size, sizeof (uint32_t));
    (void)nsae_ipc_send_block (packet, packet_size);

    log_verbose ("nsaectl: closing connection to server\n");
    nsae_ipc_free (NSAE_IPC_CLIENT);

    log_quit ();

    return 0;
}


/* vim: fdm=marker
 * end of file */
