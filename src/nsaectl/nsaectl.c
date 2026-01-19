
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
    CMD_IMP_RESET = CMD_IMP_BASE,
    CMD_IMP_BREAK,
    CMD_IMP_PRESS,
};

/* in:  element used for detecting a match.
 * out: an element used after a match is detected.
 */
struct cmd_entry { 
    const char   *long_name;    /* in */
    const char   *short_name;   /* in */
    uint_fast8_t  command_id;   /* out */
    uint_fast16_t mode_default; /* out */
    uint_fast16_t mode_options; /* in */
};

struct mode_entry { 
    const char *long_name;      /* in */
    const char *short_name;     /* in */
    uint_fast16_t mode_id;      /* out */
};

struct var_entry { 
    uint_fast16_t mode_id;      /* in */
    const char *long_name;      /* in */
    const char *short_name;     /* in */
    uint_fast16_t set_id;       /* out */
};

static const struct cmd_entry CMD_LIST[] = {
    /* {{{ */
    { "continue", "c",   CMD_CONTINUE,  MODE_NULL,   MODE_NULL },
    { "next",     "n",   CMD_NEXT,      MODE_NULL,   MODE_NULL },
    { "quit",     "q",   CMD_QUIT,      MODE_NULL,   MODE_NULL },
    { "run",      "ru",  CMD_RUN,       MODE_NULL,   MODE_NULL },
    { "step",     "st",  CMD_STEP,      MODE_NULL,   MODE_NULL },

    { "break",    "b",   CMD_IMP_BREAK, MODE_NULL,   MODE_NULL },
    { "press",    "p",   CMD_IMP_PRESS, MODE_NULL,   MODE_NULL },
    { "reset",    "res", CMD_IMP_RESET, MODE_NULL,   MODE_NULL },

    { "delete",   "d",   CMD_DELETE,    MODE_KB,   ( MODE_BR  | MODE_KB  ) },
    { "write",    "w",   CMD_WRITE,     MODE_MMU,  ( MODE_MMU | MODE_RAM ) },

    { "read",     "rea", CMD_READ,      MODE_MMU,  ( 
            MODE_FDC | MODE_HDC | MODE_MMU | MODE_RAM ) },

    { "info",     "i",   CMD_INFO,      MODE_NULL, ( 
            MODE_ADV | MODE_BR  | MODE_CPU | MODE_CRT | MODE_FDC  | MODE_HDC | 
            MODE_KB  | MODE_LOG | MODE_MMU | MODE_NSAE | MODE_RAM ) },

    { "set",      "se",  CMD_SET,       MODE_MMU,  ( 
            MODE_ADV | MODE_BR  | MODE_CPU | MODE_CRT  | MODE_FDC | MODE_HDC | 
            MODE_KB  | MODE_LOG | MODE_MMU | MODE_NSAE ) },

    { "load",     "l",   CMD_LOAD,      MODE_FDC,  ( 
            MODE_FDC | MODE_HDC | MODE_MMU | MODE_PROM | MODE_RAM ) },

    { "save",     "sa",  CMD_SAVE,      MODE_FDC,  ( 
            MODE_FDC | MODE_HDC | MODE_MMU | MODE_PROM | MODE_RAM ) },
    /* }}} */
};

static const struct mode_entry MODE_LIST[] = {
    /* {{{ */
    { "advantage",  "ad", MODE_ADV },
    { "all",        "al", MODE_ALL },
    { "breakpoint", "b",  MODE_BR },
    { "cpu",        "c",  MODE_CPU },
    { "display",    "d",  MODE_CRT },
    { "floppy",     "f",  MODE_FDC },
    { "harddrive",  "h",  MODE_HDC },
    { "keyboard",   "k",  MODE_KB },
    { "log",        "l",  MODE_LOG },
    { "mmu",        "m",  MODE_MMU },
    { "nsae",       "n",  MODE_NSAE },
    { "prom",       "p",  MODE_PROM },
    { "ram",        "r",  MODE_RAM },
    /* }}} */
};

static const struct var_entry SET_LIST[] = {
    /* {{{ */
    { MODE_BR,  "address",    "a", VAR_BR_APPEND },

    { MODE_ADV, "kbmi",       "kbm", VAR_ADV_KBMI },
    { MODE_ADV, "kbnmi",      "kbn", VAR_ADV_KBNMI },
    { MODE_ADV, "crtmi",      "cr",  VAR_ADV_CRTMI },
    { MODE_ADV, "interupt",   "i",   VAR_ADV_INTERUPT },
    { MODE_ADV, "cmdack",     "cm",  VAR_ADV_CMDACK },

    // { MODE_PORT, "", VAR_ADVANTAGE_CMDACK },

    { MODE_CPU, "a",          NULL, VAR_CPU_A },
    { MODE_CPU, "bc",         NULL, VAR_CPU_BC },
    { MODE_CPU, "de",         NULL, VAR_CPU_DE },
    { MODE_CPU, "hl",         NULL, VAR_CPU_HL },
    { MODE_CPU, "pc",         NULL, VAR_CPU_PC },
    { MODE_CPU, "sp",         NULL, VAR_CPU_SP },
    { MODE_CPU, "ix",         NULL, VAR_CPU_IX },
    { MODE_CPU, "iy",         NULL, VAR_CPU_IY },
    { MODE_CPU, "i",          NULL, VAR_CPU_I },
    { MODE_CPU, "r",          NULL, VAR_CPU_R },
    { MODE_CPU, "iff1",       NULL, VAR_CPU_IFF1 },
    { MODE_CPU, "iff2",       NULL, VAR_CPU_IFF2 },
    { MODE_CPU, "im",         NULL, VAR_CPU_IM },
    { MODE_CPU, "exx",        NULL, VAR_CPU_EXX },
    { MODE_CPU, "halt",       NULL, VAR_CPU_HALT },
    { MODE_CPU, "s_flag",     "sf", VAR_CPU_S_FLAG },
    { MODE_CPU, "z_flag",     "zf", VAR_CPU_Z_FLAG },
    { MODE_CPU, "h_flag",     "hf", VAR_CPU_H_FLAG },
    { MODE_CPU, "p_flag",     "pf", VAR_CPU_P_FLAG },
    { MODE_CPU, "v_flag",     "vf", VAR_CPU_V_FLAG },
    { MODE_CPU, "n_flag",     "nf", VAR_CPU_N_FLAG },
    { MODE_CPU, "c_flag",     "cf", VAR_CPU_C_FLAG },

    { MODE_CRT, "color",      "c", VAR_CRT_COLOR },
    { MODE_CRT, "colour",     "c", VAR_CRT_COLOR },
    { MODE_CRT, "blank",      "b", VAR_CRT_BLANK },
    { MODE_CRT, "vsync",      "v", VAR_CRT_VSYNC },
    { MODE_CRT, "scroll",     "s", VAR_CRT_SCROLL },

    { MODE_FDC, "disk",       "d",   VAR_FDC_DISK },
    { MODE_FDC, "side",       "si",  VAR_FDC_SIDE },
    { MODE_FDC, "track",      "t",   VAR_FDC_TRACK },
    { MODE_FDC, "powered",    "p",   VAR_FDC_POWERED },
    { MODE_FDC, "zerotrack",  "z",   VAR_FDC_TRACKZERO },
    { MODE_FDC, "sectormark", "se",  VAR_FDC_SECTORMARK },
    { MODE_FDC, "0:eject",    "0:e", VAR_FDC_EJECT_A },
    { MODE_FDC, "1:eject",    "1:e", VAR_FDC_EJECT_B },
    { MODE_FDC, "0:sector",   "0:s", VAR_FDC_SECTOR_A },
    { MODE_FDC, "1:sector",   "1:s", VAR_FDC_SECTOR_B },

    { MODE_HDC, "eject",      "e", VAR_HDC_EJECT },

    { MODE_KB,  "repeat",     "r",  VAR_KB_REPEAT },
    { MODE_KB,  "capslock",   "ca", VAR_KB_CAPSLOCK },
    { MODE_KB,  "cursorlock", "cu", VAR_KB_CURSORLOCK },
    { MODE_KB,  "overflow",   "o",  VAR_KB_OVERFLOW },
    { MODE_KB,  "dataflag",   "d",  VAR_KB_DATAFLAG },
    { MODE_KB,  "interrupt",  "i",  VAR_KB_INTERRUPT },
    { MODE_KB,  "press",      "p",  VAR_KB_PRESS },

    { MODE_MMU, "slot0",      "0", VAR_MMU_SLOT0 },
    { MODE_MMU, "slot1",      "1", VAR_MMU_SLOT1 },
    { MODE_MMU, "slot2",      "2", VAR_MMU_SLOT2 },
    { MODE_MMU, "slot3",      "3", VAR_MMU_SLOT3 },

    { MODE_LOG, "display",    "d",  VAR_LOG_CRT},
    { MODE_LOG, "cpu",        "c",  VAR_LOG_CPU },
    { MODE_LOG, "keyboard",   "k",  VAR_LOG_KB },
    { MODE_LOG, "ram",        "r",  VAR_LOG_RAM },
    { MODE_LOG, "mmu",        "m",  VAR_LOG_MMU },
    { MODE_LOG, "floppy",     "f",  VAR_LOG_FDC },
    { MODE_LOG, "harddrive",  "h",  VAR_LOG_HDC },
    { MODE_LOG, "advantage",  "ad", VAR_LOG_ADV },
    { MODE_LOG, "all",        "al", VAR_LOG_ALL },
    { MODE_LOG, "logfile",    "l",  VAR_LOG_OUTPUT_FILE },
    /* }}} */
};


static void version (void);
static void usage (void);
static int send (int argc, char **argv);

static char *deref_filepath (const char *src, size_t *ret_size);

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

static char *
deref_filepath (const char *src, size_t *ret_len)
{
    size_t src_len = 0;
    const char *pwd = NULL;
    char *dst = NULL;
    size_t dst_len = 0;

    src_len = strlen (src);

    /* check if absolute path */
    if (*src == '/') 
    {
        *ret_len = src_len;
        return strdup (src);
    }

    /* treat as relative */
    pwd = getcwd (NULL, 0);
    dst_len = strlen (pwd) + 1 + src_len;
    dst = calloc (dst_len + 1, sizeof (char));

    strcat (dst, pwd);
    strcat (dst, "/");
    strcat (dst, src);

    *ret_len = dst_len;
    return dst;
}

static int
match_name (const char *input, const char *long_name, const char *short_name)
{
    return ((0 == strcmp (input, long_name)) ||
            (short_name != NULL && 0 == strcmp (input, short_name)));
}

static const struct cmd_entry *
get_cmd (const char *input)
{
    size_t i = 0;
    const struct cmd_entry *iter = NULL;

    for (i = 0; i < ARRLEN (CMD_LIST); i++)
    {
        iter = &CMD_LIST[i];
        if (match_name (input, iter->long_name, iter->short_name))
        {
            return iter;
        }
    }

    return NULL;
}

static int_fast32_t
get_mode (const struct cmd_entry *cmd, const char *input, int *ret_arg_is_mode)
{
    const size_t MODE_LIST_LEN = ARRLEN (MODE_LIST);

    size_t i = 0;
    const struct mode_entry *iter = NULL;
    int_fast32_t mode_id = -1;

    /* sanity checks:
     * - no parameters may be NULL
     * - command should expect a mode */
    assert (cmd != NULL);
    assert (input != NULL);
    assert (ret_arg_is_mode != NULL);

    if (cmd->mode_options == MODE_NULL) 
    {
        mode_id = cmd->mode_default;
        goto early_exit;
    }

    /* find mode */
    for (i = 0; i < MODE_LIST_LEN; i++)
    {
        iter = &MODE_LIST[i];
        if (match_name (input, iter->long_name, iter->short_name))
        {
            mode_id = iter->mode_id;
            break;
        }
    }

    /* no mode was found */
    if (i == MODE_LIST_LEN)
    {
        iter = NULL;

        /* use default mode if present, otherwise error out */
        if (cmd->mode_default != MODE_NULL)
        {
            mode_id = cmd->mode_default;
            log_verbose ("nsaectl: no mode given, using default -- %s\n",
                    mode_id);
        }
        else
        {
            log_error ("nsaectl: unknown mode -- %s\n", input);
        }
        goto early_exit;
    }

    /* check if mode is supported by command */
    /* NOTE: at this point we know that the for loop succeeded. meaning iter 
     *       and mode_id should match */
    assert (i != MODE_LIST_LEN);
    assert (iter != NULL);
    assert (mode_id != -1);
    assert (iter->mode_id == (uint_fast16_t)mode_id);

    if (!(cmd->mode_options & mode_id))
    {
        log_error ("nsaectl: %s does not support mode -- %s\n",
                cmd->long_name, iter->long_name);
        mode_id = -1;
        /* goto early_exit; */
    }

early_exit:
    *ret_arg_is_mode = (iter != NULL);
    return mode_id;
}

static int 
send (int argc, char **argv)
{
    size_t i = 0;
    char *cmd_name = NULL;

    const struct cmd_entry *cmd_entry = NULL;
    int arg_is_mode = 0;
    int_fast32_t ret_mode_id = 0;

    nsae_packet_t packet = { 0 };
    nsae_packet_t *heap_packet = NULL;
    size_t heap_packet_size = 0;

    char *iter = NULL;
    char *stmp = NULL;
    size_t stmp_len = 0;

    char *input_format = NULL;
    /* char *output_format = NULL; */
    
    memset (&packet, 0, sizeof (packet));

    /* get command */
    cmd_name = ARG_POP ();
    cmd_entry = get_cmd (cmd_name);
    if (cmd_entry == NULL)
    {
        log_fatal ("nsaectl: invalid command -- %s\n", cmd_name);
        return 1;
    }
    packet.cmd = cmd_entry->command_id;

    /* get mode if necessary */
    if (argc > 0)
    {
        ret_mode_id = get_mode (cmd_entry, *argv, &arg_is_mode);
        if (ret_mode_id == -1)
        {
            /* throws error internally */
            return 1;
        }
        else
        {
            packet.mode = ret_mode_id;
        }

        if (arg_is_mode)
        {
            ARG_POP ();
        }
    }

    /* command handling */
    switch (packet.cmd)
    {
    case CMD_STEP:
        packet.v_count = (argc <= 0) ? 1 : strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_DELETE:
        packet.v_index = (argc <= 0) ? 1 : strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_QUIT:
    case CMD_NEXT:
    case CMD_CONTINUE:
    case CMD_INFO:
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_RUN:
        if (argc > 0)
        {
            packet.cmd  = CMD_SET;
            packet.mode = MODE_CPU;
            packet.var  = VAR_CPU_PC;
            packet.v_addr32 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (&packet, sizeof (packet));
        }

        packet.cmd = CMD_RUN;
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_SET:
        stmp = ARG_POP ();
        for (i = 0; i < ARRLEN (SET_LIST); i++)
        {
            if (packet.mode !=  SET_LIST[i].mode_id) continue;
            if (!match_name (stmp, SET_LIST[i].long_name, SET_LIST[i].short_name))
            {
                continue;
            }

            packet.var = SET_LIST[i].set_id;
            packet.v_data32 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (&packet, sizeof (packet));
            break;
        }

        if (i == ARRLEN (SET_LIST))
        {
            /* unknown variable name */
            log_fatal ("unknown variable name -- %s", stmp);
            return 1;
        }

        break;

    case CMD_LOAD:
    case CMD_SAVE:
        packet.v_addr32  = strtol (ARG_POP (), NULL, 0);
        packet.v_fddrive = strtol (ARG_POP (), NULL, 0);

        stmp = ARG_POP (); /* filename */
        stmp_len = strlen (stmp);

        stmp = deref_filepath (stmp, &stmp_len);

        heap_packet_size = sizeof (packet) + stmp_len + 1;
        heap_packet = calloc (1, heap_packet_size);
        assert (heap_packet != NULL);

        memcpy (heap_packet, &packet, sizeof (packet));
        memcpy (heap_packet->buf, stmp, stmp_len);
        nsae_ipc_send (heap_packet, heap_packet_size);

        free (stmp);
        stmp = NULL;
        stmp_len = 0;

        free (heap_packet);
        break;

    case CMD_READ:
        stmp = ARG_POP ();

        /* handle floppy format drive:sector */
        if (packet.mode == MODE_FDC)
        {
            packet.v_data8 = strtol (stmp, NULL, 0);

            iter = strchr (stmp, ':');
            if (iter == NULL)
            {
                /* missing drive select */
                log_fatal ("missing drive select");
                exit (EXIT_FAILURE);
            }

            packet.v_addr32 = strtol (iter + 1, NULL, 0);
        }
        else /* handle standard format */
        {
            packet.v_addr32 = strtol (stmp, NULL, 0);
        }

        packet.v_addr32 = (argc <= 0) ? 0x100 : strtol (ARG_POP (), NULL, 0);
        //format = (argc <= 0) ? "byte" : ARG_POP ();
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_WRITE:
        /* get address */
        packet.v_addr32 = strtol (ARG_POP (), NULL, 0);

        /* send data */
        do {
            packet.v_data8 = strtol (ARG_POP (), NULL, 0);
            nsae_ipc_send (&packet, sizeof (packet));

            packet.v_addr32++;
        } while (argc > 0);

        break;

    case CMD_IMP_RESET:
        packet.cmd  = CMD_SET;
        packet.mode = MODE_MMU;
        packet.var  = VAR_MMU_SLOT0;
        packet.v_data8 = 0x8;
        nsae_ipc_send (&packet, sizeof (packet));

        packet.var = VAR_MMU_SLOT1;
        packet.v_data8 = 0x9;
        nsae_ipc_send (&packet, sizeof (packet));

        packet.var = VAR_MMU_SLOT2;
        packet.v_data8 = 0xc;
        nsae_ipc_send (&packet, sizeof (packet));

        packet.var = VAR_MMU_SLOT3;
        packet.v_data8 = 0x0;
        nsae_ipc_send (&packet, sizeof (packet));

        packet.cmd  = CMD_SET;
        packet.mode = MODE_CPU;
        packet.var  = VAR_CPU_PC;
        packet.v_addr32 = 0x8000;
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_IMP_BREAK:
        packet.cmd    = CMD_SET;
        packet.mode   = MODE_BR;
        packet.var    = VAR_BR_APPEND;
        packet.v_addr32 = strtol (ARG_POP (), NULL, 0);
        nsae_ipc_send (&packet, sizeof (packet));
        break;

    case CMD_IMP_PRESS:
        packet.cmd  = CMD_SET;
        packet.mode = MODE_KB;
        packet.var  = VAR_KB_PRESS;

        /* get format and data */
        stmp = ARG_POP ();
        input_format = NULL;
        if (argc > 0)
        {
            input_format = ARG_POP ();
        }

        /* send data based on format */
        if ((input_format == NULL) || (0 == strcmp (input_format, "keycode")))
        {
            packet.v_data8 = strtol (stmp, NULL, 0);
            nsae_ipc_send (&packet, sizeof (packet));
        }
        else if (0 == strcmp (input_format, "string"))
        {
            for (; *stmp; stmp++)
            {
                packet.v_data8 = *stmp;
                nsae_ipc_send (&packet, sizeof (packet));
            }
        }
        else
        {
            /* error unknown format */
            log_fatal ("unknown format specifier -- %s", input_format);
            exit (EXIT_FAILURE);
        }
        break;

    default:
        /* unknown command */
        log_fatal ("unknown command id -- %d", packet.cmd);
        exit (EXIT_FAILURE);
    }

    return 0;
}











#if 0
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
    assert (p_mode->cmd <= CMD_COUNT);

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
    case CMD_BRKPNT_SET:
    case CMD_BRKPNT_REMOVE:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        break;

    case CMD_WP_SET:
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

    case CMD_WP_REMOVE:
        packet->v_data = strtol (argv[0], NULL, 0);
        break;

    case CMD_LOG_CPU:
    case CMD_LOG_MMU:
    case CMD_LOG_RAM:
    case CMD_LOG_FDC:
    case CMD_LOG_CRT:
    case CMD_LOG_KB:
    case CMD_LOG_MOBO:
    case CMD_KB_OVERFLOW:
    case CMD_KB_CAPS:
    case CMD_KB_CURSOR:
    case CMD_KB_DATA:
    case CMD_KB_INTERUPT:
        packet->v_state = strtol (argv[0], NULL, 0);
        break;

    case CMD_LOG_OUTPUT:
    case CMD_HD_LOAD:
    case CMD_HD_SAVE:
    case CMD_PROM_LOAD:
        packet->v_buflen = strlen (argv[0]);
        packet_size += packet->v_buflen;
        packet = realloc (packet, packet_size);
        memcpy (packet->buf, argv[0], packet->v_buflen);
        break;
    
    case CMD_FD_EJECT:
        packet->v_fd_num = strtol (argv[0], NULL, 0);
        break;

    case CMD_FD_LOAD:
    case CMD_FD_SAVE:
        packet->v_fd_num = strtol (argv[0], NULL, 0);
        packet->v_buflen = strlen (argv[1]);
        packet_size += packet->v_buflen;
        packet = realloc (packet, packet_size);
        memcpy (packet->buf, argv[1], packet->v_buflen);
        break;

    case CMD_FD_BLK_READ:
        packet->v_fd_num    = strtol (argv[0], NULL, 0);
        packet->v_fd_side   = strtol (argv[1], NULL, 0);
        packet->v_fd_track  = strtol (argv[2], NULL, 0);
        packet->v_fd_sector = strtol (argv[3], NULL, 0);
        break;

    case CMD_KB_PUSH:
        packet->v_keycode = strtol (argv[0], NULL, 0);
        break;

    case CMD_ADV_OUT:
        packet->v_port = strtol (argv[0], NULL, 0);
        packet->v_data = strtol (argv[1], NULL, 0);
        break;

    case CMD_ADV_IN:
        packet->v_port = strtol (argv[0], NULL, 0);
        break;

    case CMD_RAM_READ:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        packet->v_span32 = (argc < 1) ? 1 : strtol (argv[1], NULL, 0);
        break;

    case CMD_RAM_WRITE:
        packet->v_addr32 = strtol (argv[0], NULL, 0);
        packet->v_data   = strtol (argv[1], NULL, 0);
        break;

    case CMD_MMU_READ:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        packet->v_span16 = (argc < 1) ? 1 : strtol (argv[1], NULL, 0);
        break;

    case CMD_MMU_WRITE:
        packet->v_addr16 = strtol (argv[0], NULL, 0);
        packet->v_data   = strtol (argv[1], NULL, 0);
        break;

    case CMD_MMU_LOAD:
        packet->v_slot = strtol (argv[0], NULL, 0);
        packet->v_page = strtol (argv[1], NULL, 0);
        break;

    case CMD_EXIT:
    case CMD_RESTART:
    case CMD_PAUSE:
    case CMD_CONTINUE:
    case CMD_BRKPNT_LIST:
    case CMD_WP_LIST:
    case CMD_STEP:
    case CMD_RUN:
    case CMD_STATUS:
    case CMD_LOG_TERSE:
    case CMD_LOG_VERBOSE:
    case CMD_LOG_DEBUG:
    case CMD_HD_EJECT:
    case CMD_KB_POP:
    case CMD_KB_STATUS:
    case CMD_IO_STATUS:
    case CMD_CRT_STATUS:
    case CMD_ADV_STATUS:
    case CMD_CPU_STATUS:
    case CMD_MMU_STATUS:
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


#endif
/* vim: fdm=marker
 * end of file */
