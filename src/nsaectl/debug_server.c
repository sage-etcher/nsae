
#include "nsaecmd.h"
#include "nsaeipc.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>


int
main (void)
{
#define BUF_MAX 128
    char buf[BUF_MAX+1];
    int exit_flag = 0;

    nsae_ipc_init (NSAE_IPC_SERVER);
    while (!exit_flag)
    {
        uint8_t cmd = 0;
        int rc = nsae_ipc_recieve ((uint8_t *)&cmd, sizeof (uint8_t));
        if (rc == 0) 
        {
            sleep (1);
            continue; 
        }

        uint8_t fd_num = 0;
        size_t filename_len = 0;
        uint32_t abs_addr = 0;

        switch (cmd)
        {
        case NSAE_CMD_EXIT: 
            exit_flag = 1; 
            puts ("exit"); 
            break;

        case NSAE_CMD_EJECT_FD:
            nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t));
            printf ("eject_fd %u\n", fd_num);
            break;

        case NSAE_CMD_LOAD_FD:
        case NSAE_CMD_SAVE_FD:
            nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t));
            nsae_ipc_recieve ((uint8_t *)&filename_len, sizeof (size_t));
            nsae_ipc_recieve ((uint8_t *)buf, filename_len);
            buf[BUF_MAX] = '\0';

            printf ("%s %u %zu %s\n", 
                    (cmd == NSAE_CMD_LOAD_FD ? "load_fd" : "save_fd"),
                    fd_num, filename_len, buf);
            break;

        case NSAE_CMD_LOAD_HD:
        case NSAE_CMD_SAVE_HD:
            nsae_ipc_recieve ((uint8_t *)&filename_len, sizeof (size_t));
            nsae_ipc_recieve ((uint8_t *)buf, filename_len);

            printf ("%s %zu %s\n", 
                    (cmd == NSAE_CMD_LOAD_HD ? "load_hd" : "save_hd"),
                    filename_len, buf);
            break;

        case NSAE_CMD_BREAKPOINT: 
            nsae_ipc_recieve ((uint8_t *)&abs_addr, sizeof (uint32_t));

            printf ("breakpoint %08x\n", abs_addr);
            break;

        case NSAE_CMD_RESTART: puts ("restart"); break;
        case NSAE_CMD_PAUSE: puts ("pause"); break;
        case NSAE_CMD_CONTINUE: puts ("continue"); break;
        case NSAE_CMD_EJECT_HD: puts ("eject hd"); break;
        case NSAE_CMD_STEP: puts ("step"); break;
        case NSAE_CMD_RUN: puts ("run"); break;
        case NSAE_CMD_STATUS: puts ("status"); break;
        }
    }

    nsae_ipc_free (NSAE_IPC_SERVER);
    return 0;
}
