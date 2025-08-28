
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

    nsae_ipc_init (NSAE_IPC_SERVER, NULL, NULL);
    while (!exit_flag)
    {
        uint8_t cmd = 0;
        int rc = nsae_ipc_recieve ((uint8_t *)&cmd, sizeof (uint8_t));
        if (rc <= 0) 
        {
            sleep (1);
            continue; 
        }

        uint8_t fd_num = 0;
        uint8_t fd_side = 0;
        uint8_t fd_track = 0;
        uint8_t fd_sec = 0;
        uint8_t kb_state = 0;
        uint8_t kb_keycode = 0;
        size_t filename_len = 0;
        uint8_t slot = 0;
        uint8_t page = 0;
        uint16_t addr = 0;
        uint32_t abs_addr = 0;
        uint8_t data = 0;

        switch (cmd)
        {
        case NSAE_CMD_EXIT: 
            exit_flag = 1; 
            puts ("exit"); 
            break;

        case NSAE_CMD_BRKPNT_SET:
        case NSAE_CMD_BRKPNT_REMOVE:
            nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));

            printf ("breakpoint %s %04x\n", 
                    (cmd == NSAE_CMD_BRKPNT_SET    ? "set"    :
                     cmd == NSAE_CMD_BRKPNT_REMOVE ? "remove" : "unknown"),
                    (unsigned)addr);
            break;

        case NSAE_CMD_BRKPNT_LIST:
            printf ("breakpoint list\n");
            break;

        case NSAE_CMD_FD_EJECT:
        case NSAE_CMD_FD_STATUS:
            nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
            printf ("%s %u\n",
                    (cmd == NSAE_CMD_FD_EJECT ? "fd_eject" : "fd_status"),
                    fd_num);
            break;

        case NSAE_CMD_FD_BLK_READ:
            nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
            nsae_ipc_recieve_block ((uint8_t *)&fd_side, sizeof (uint8_t));
            nsae_ipc_recieve_block ((uint8_t *)&fd_track, sizeof (uint8_t));
            nsae_ipc_recieve_block ((uint8_t *)&fd_sec, sizeof (uint8_t));

            printf ("fd_blkread %d %d %d %d\n", 
                    (int)fd_num, (int)fd_side, (int)fd_track, (int)fd_sec);
            break;

        case NSAE_CMD_FD_LOAD:
        case NSAE_CMD_FD_SAVE:
            nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
            nsae_ipc_recieve_block ((uint8_t *)&filename_len, sizeof (size_t));
            nsae_ipc_recieve_block ((uint8_t *)buf, filename_len);
            buf[BUF_MAX] = '\0';

            printf ("%s %u %zu %s\n", 
                    (cmd == NSAE_CMD_FD_LOAD ? "load_fd" :
                     cmd == NSAE_CMD_FD_SAVE ? "save_fd" : "unknown"),
                    (unsigned)fd_num, filename_len, buf);
            break;

        case NSAE_CMD_HD_LOAD:
        case NSAE_CMD_HD_SAVE:
        case NSAE_CMD_PROM_LOAD:
            nsae_ipc_recieve_block ((uint8_t *)&filename_len, sizeof (size_t));
            nsae_ipc_recieve_block ((uint8_t *)buf, filename_len);

            printf ("%s %zu %s\n", 
                    (cmd == NSAE_CMD_HD_LOAD   ? "load_hd"   :
                     cmd == NSAE_CMD_HD_SAVE   ? "save_hd"   :
                     cmd == NSAE_CMD_PROM_LOAD ? "prom_load" : "unknown"),
                    filename_len, buf);
            break;

        case NSAE_CMD_KB_PUSH:
            nsae_ipc_recieve_block ((uint8_t *)&kb_keycode, sizeof (uint8_t));
            printf ("kb_push %02x\n", (unsigned)kb_keycode);
            break;


        case NSAE_CMD_KB_OVERFLOW:
        case NSAE_CMD_KB_CAPS:
        case NSAE_CMD_KB_CURSOR:
        case NSAE_CMD_KB_DATA:
        case NSAE_CMD_KB_INTERUPT:
            nsae_ipc_recieve_block ((uint8_t *)&kb_state, sizeof (uint8_t));
            printf ("%s %d\n", 
                    (cmd == NSAE_CMD_KB_OVERFLOW ? "kb_overflow" :
                     cmd == NSAE_CMD_KB_CAPS     ? "kb_caps"     :
                     cmd == NSAE_CMD_KB_CURSOR   ? "kb_cursor"   :
                     cmd == NSAE_CMD_KB_DATA     ? "kb_data"     :
                     cmd == NSAE_CMD_KB_INTERUPT ? "kb_interupt" : "unknown"),
                    (int)kb_state);
            break;

        case NSAE_CMD_RAM_READ:
            nsae_ipc_recieve_block ((uint8_t *)&abs_addr, sizeof (uint32_t));
            printf ("ram_read %08x\n", abs_addr);
            break;

        case NSAE_CMD_RAM_WRITE:
            nsae_ipc_recieve_block ((uint8_t *)&abs_addr, sizeof (uint32_t));
            nsae_ipc_recieve_block ((uint8_t *)&data, sizeof (uint8_t));
            printf ("ram_write %08x %02x\n", abs_addr, data);
            break;

        case NSAE_CMD_MMU_READ:
            nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
            printf ("mmu_read %04x\n", addr);
            break;

        case NSAE_CMD_MMU_WRITE:
            nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
            nsae_ipc_recieve_block ((uint8_t *)&data, sizeof (uint8_t));
            printf ("mmu_write %04x %02x\n", addr, data);
            break;

        case NSAE_CMD_MMU_LOAD:
            nsae_ipc_recieve_block ((uint8_t *)&slot, sizeof (uint8_t));
            nsae_ipc_recieve_block ((uint8_t *)&page, sizeof (uint8_t));
            printf ("mmu_load %x %x\n", (unsigned)slot, (unsigned)page);
            break;

        case NSAE_CMD_RESTART:      puts ("restart"); break;
        case NSAE_CMD_PAUSE:        puts ("pause"); break;
        case NSAE_CMD_CONTINUE:     puts ("continue"); break;
        case NSAE_CMD_STEP:         puts ("step"); break;
        case NSAE_CMD_RUN:          puts ("run"); break;
        case NSAE_CMD_STATUS:       puts ("status"); break;
        case NSAE_CMD_HD_EJECT:     puts ("eject hd"); break;
        case NSAE_CMD_KB_POP:       puts ("kb_pop"); break;
        case NSAE_CMD_KB_STATUS:    puts ("kb_status"); break;
        case NSAE_CMD_IO_STATUS:    puts ("io_status"); break;
        case NSAE_CMD_CRT_STATUS:   puts ("crt_status"); break;
        case NSAE_CMD_ADV_STATUS:   puts ("adv_status"); break;
        case NSAE_CMD_CPU_STATUS:   puts ("cpu_status"); break;
        case NSAE_CMD_MMU_STATUS:   puts ("mmu_status"); break;

        default:
            printf ("unkown command 0x%02x", cmd);
            return 1;
        }


    }

    nsae_ipc_free (NSAE_IPC_SERVER);
    return 0;
}
