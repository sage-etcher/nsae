
#include "client.h"

#include "log.h"
#include "nsaeipc.h"
#include "nsaecmd.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void vsend_cmd (uint8_t cmd, va_list args);

static void send_byte (uint8_t b);
static void send_string (char *s);
static void send_u16 (uint16_t u);
static void send_u32 (uint32_t u);


void
send_cmd (uint8_t cmd, ...)
{
    va_list args;
    va_start (args, cmd);
    vsend_cmd (cmd, args);
    va_end (args);
}

static void
vsend_cmd (uint8_t cmd, va_list args)
{
    switch (cmd)
    {
        /* cmd u32 */
        case NSAE_CMD_RAM_READ:     /* cmd abs_addr */
            send_byte (cmd);
            send_u32 ((uint32_t)va_arg (args, int));
            break;

        /* cmd u16 */
        case NSAE_CMD_BRKPNT_SET:   /* cmd addr */
        case NSAE_CMD_BRKPNT_REMOVE:/* cmd addr */
        case NSAE_CMD_MMU_READ:     /* cmd addr */
            send_byte (cmd);
            send_u16 ((uint16_t)va_arg (args, int));
            break;

        /* cmd u8 */
        case NSAE_CMD_FD_EJECT:     /* cmd fd_num */
        case NSAE_CMD_FD_STATUS:    /* cmd fd_num */
        case NSAE_CMD_KB_PUSH:      /* cmd keycode */
        case NSAE_CMD_KB_OVERFLOW:  /* cmd state */
        case NSAE_CMD_KB_CAPS:      /* cmd state */
        case NSAE_CMD_KB_CURSOR:    /* cmd state */
        case NSAE_CMD_KB_DATA:      /* cmd state */
        case NSAE_CMD_KB_INTERUPT:  /* cmd state */
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd u32 u8 */
        case NSAE_CMD_RAM_WRITE:    /* cmd abs_addr data */
            send_byte (cmd);
            send_u32 ((uint32_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd u16 u8 */
        case NSAE_CMD_MMU_WRITE:    /* cmd addr data */
            send_byte (cmd);
            send_u16 ((uint16_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd u8 u8 */
        case NSAE_CMD_MMU_LOAD:     /* cmd slot page */
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd u8 u8 u8 u8 */
        case NSAE_CMD_FD_BLK_READ:  /* cmd fd_num side track sec */
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd u8 size str */
        case NSAE_CMD_FD_LOAD:  /* cmd fd_num file */
        case NSAE_CMD_FD_SAVE:  /* cmd fd_num file */
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            send_string (va_arg (args, char *));
            break;

        /* cmd u8 size str */
        case NSAE_CMD_HD_LOAD:      /* cmd len file */
        case NSAE_CMD_HD_SAVE:      /* cmd len file */
        case NSAE_CMD_PROM_LOAD:    /* cmd len file */
            send_byte (cmd);
            send_string (va_arg (args, char *));
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
        case NSAE_CMD_HD_EJECT:
        case NSAE_CMD_KB_POP:
        case NSAE_CMD_KB_STATUS:
        case NSAE_CMD_IO_STATUS:
        case NSAE_CMD_CRT_STATUS:
        case NSAE_CMD_ADV_STATUS:
        case NSAE_CMD_CPU_STATUS:
        case NSAE_CMD_MMU_STATUS:
            send_byte (cmd);
            break;

        /* error */
        default: 
            log_error ("nsaectl: error: unknown command\n");
            abort ();
    }
}

static void
send_byte (uint8_t b)
{
    log_verbose ("nsaectl: send: (u8)%u\n", b);
    nsae_ipc_send (&b, sizeof (uint8_t));
}

static void
send_string (char *s)
{
    size_t len = strlen (s);
    log_verbose ("nsaectl: send: (size_t)%zu '%s'\n", len, s);
    nsae_ipc_send ((uint8_t *)&len, sizeof (size_t));
    nsae_ipc_send ((uint8_t *)s, len);
}

static void
send_u16 (uint16_t u)
{
    log_verbose ("nsaectl: send: (u16)%u\n", u);
    nsae_ipc_send ((uint8_t *)&u, sizeof (uint16_t));
}

static void
send_u32 (uint32_t u)
{
    log_verbose ("nsaectl: send: (u32)%u\n", u);
    nsae_ipc_send ((uint8_t *)&u, sizeof (uint32_t));
}

/* end of file */
