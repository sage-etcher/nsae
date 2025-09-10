
#include "nsaecmd.h"
#include "nsaeipc.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

int
main (void)
{
    int rc = 0;
    int exit_flag = 0;

    uint32_t packet_size = 0;
    nsae_packet_t *packet = NULL;


    nsae_ipc_init (NSAE_IPC_SERVER, NULL, NULL);

    while (!exit_flag)
    {
        /* wait for packet_size */
        packet_size = 0;
        rc = nsae_ipc_recieve (&packet_size, sizeof (uint32_t));
        if (rc <= 0)
        {
            sleep (1);
            continue;
        }

        /* enforce minimum packet size */
        if (packet_size < sizeof (nsae_packet_t))
        {
            printf ("packet_size cannot be less than %zu\n", 
                    sizeof (nsae_packet_t));
            exit_flag = 1;
            continue;
        }

        /* wait for full packet */
        packet = realloc (packet, packet_size);
        rc = nsae_ipc_recieve_block (packet, packet_size);

        /* enforce that (sizeof (nsae_packet_t) + buflen) == packet_size */
        switch (packet->cmd)
        {
        case NSAE_CMD_LOG_OUTPUT:
        case NSAE_CMD_FD_LOAD:
        case NSAE_CMD_FD_SAVE:
        case NSAE_CMD_HD_LOAD:
        case NSAE_CMD_HD_SAVE:
        case NSAE_CMD_PROM_LOAD:
            if (packet_size != (packet->v_buflen + sizeof (nsae_packet_t)))
            {
                pritnf ("packet_size buflen mismatch\n");
                exit_flag = 1;
                continue;
            }
            break;
        }

        /* evaluate the packet */
        switch (packet->cmd)
        {
        case NSAE_CMD_NOP:
        case NSAE_CMD_RESTART:
        case NSAE_CMD_PAUSE:
        case NSAE_CMD_CONTINUE:
        case NSAE_CMD_BRKPNT_LIST:
        case NSAE_CMD_STEP:
        case NSAE_CMD_RUN:
        case NSAE_CMD_STATUS:
        case NSAE_CMD_TERSE:
        case NSAE_CMD_VERBOSE:
        case NSAE_CMD_DEBUG:
        case NSAE_CMD_HD_EJECT:
        case NSAE_CMD_KB_POP:
        case NSAE_CMD_KB_STATUS:
        case NSAE_CMD_IO_STATUS:
        case NSAE_CMD_CRT_STATUS:
        case NSAE_CMD_ADV_STATUS:
        case NSAE_CMD_MMU_STATUS:
            printf ("%02x\n", packet->cmd);
            break;

        case NSAE_CMD_EXIT:
            exit_flag = 1;
            continue;

        case NSAE_CMD_BRKPNT_SET:
        case NSAE_CMD_BRKPNT_REMOVE:
            printf ("%02x %04x\n", packet->cmd, packet->v_addr16);
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
            printf ("%02x %02x\n", packet->cmd, packet->v_state);
            break;

        case NSAE_CMD_LOG_OUTPUT:
        case NSAE_CMD_HD_LOAD:
        case NSAE_CMD_HD_SAVE:
        case NSAE_CMD_PROM_LOAD:
            printf ("%02x %d %.*s\n",
                    packet->cmd,
                    packet->v_buflen,
                    packet->v_buflen, packet->buf);
            break;

        case NSAE_CMD_FD_EJECT:
            printf ("%02x %02x\n", packet->cmd, packet->v_fd_num);
            break;

        case NSAE_CMD_FD_LOAD:
        case NSAE_CMD_FD_SAVE:
            printf ("%02x %02x %d %.*s\n", 
                    packet->cmd, 
                    packet->v_fd_num,
                    packet->v_buflen,
                    packet->v_buflen, packet->buf);
            break;

        case NSAE_CMD_KB_PUSH:
            printf ("%02x %02x '%c'\n",
                    packet->cmd,
                    packet->v_keycode,
                    packet->v_keycode);
            break;

        case NSAE_CMD_ADV_OUT:
            printf ("%02x %02x %02x\n",
                    packet->cmd,
                    packet->v_port,
                    packet->v_data);
            break;

        case NSAE_CMD_ADV_IN:
            printf ("%02x %02x\n", packet->cmd, packet->v_port);
            break;

        case NSAE_CMD_RAM_READ:
            printf ("%02x %08x %08x\n",
                    packet->cmd,
                    packet->v_addr32,
                    packet->v_span32);
            break;

        case NSAE_CMD_RAM_WRITE:
            printf ("%02x %08x %02x\n",
                    packet->cmd,
                    packet->v_addr32,
                    packet->v_data);
            break;

        case NSAE_CMD_MMU_READ:
            printf ("%02x %04x %04x\n",
                    packet->cmd,
                    packet->v_addr16,
                    packet->v_span16);
            break;

        case NSAE_CMD_RAM_WRITE:
            printf ("%02x %04x %02x\n",
                    packet->cmd,
                    packet->v_addr16,
                    packet->v_data);
            break;

        case NSAE_CMD_MMU_LOAD:
            printf ("%02x %02x %02x\n",
                    packet->cmd,
                    packet->v_slot,
                    packet->v_page);
            break;

        default:
            printf ("unkown command %02x", cmd);
            exit_flag = 1;
            break;
        }
    }

    free (packet);

    nsae_ipc_free (NSAE_IPC_SERVER);

    return 0;
}

/* end of file */
