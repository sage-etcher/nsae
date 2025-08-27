
#include "server.h"

#include "adv.h"
#include "fdc.h"
#include "log.h"
#include "mmu.h"
#include "nsae.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "ram.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
server_handle_ipc (nsae_t *self)
{
    uint8_t cmd = 0x00;
    uint8_t fd_num = 0x00;
    uint8_t fd_side = 0x00;
    uint8_t fd_track = 0x00;
    uint8_t fd_sector = 0x00;
    uint8_t kb_state = 0x00;
    uint8_t kb_keycode = 0x00;
    uint8_t mmu_page = 0x00;
    uint8_t mmu_slot = 0x00;
    uint8_t data = 0x00;
    uint16_t addr = 0x0000;
    uint32_t abs_addr = 0x00000000;
    char *file = NULL;
    size_t n = 0;

    int rc = nsae_ipc_recieve (&cmd, sizeof (uint8_t));
    if (rc == 0) return 0; /* non blocking */

    /* get parameters */
    switch (cmd)
    {
    case NSAE_CMD_BREAKPOINT:
        nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
        break;

    case NSAE_CMD_FD_EJECT:
    case NSAE_CMD_FD_STATUS:
        nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
        break;

    case NSAE_CMD_FD_BLK_READ:
        nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
        nsae_ipc_recieve_block ((uint8_t *)&fd_side, sizeof (uint8_t));
        nsae_ipc_recieve_block ((uint8_t *)&fd_track, sizeof (uint8_t));
        nsae_ipc_recieve_block ((uint8_t *)&fd_sector, sizeof (uint8_t));
        break;


    case NSAE_CMD_FD_LOAD:
    case NSAE_CMD_FD_SAVE:
        nsae_ipc_recieve_block ((uint8_t *)&fd_num, sizeof (uint8_t));
        /* fall through */
    case NSAE_CMD_HD_LOAD:
    case NSAE_CMD_HD_SAVE:
    case NSAE_CMD_PROM_LOAD:
        nsae_ipc_recieve_block ((uint8_t *)&n, sizeof (size_t));
        file = malloc (n+1);
        nsae_ipc_recieve_block ((uint8_t *)file, n);
        file[n] = '\0';
        break;

    case NSAE_CMD_KB_PUSH:
        nsae_ipc_recieve_block ((uint8_t *)&kb_keycode, sizeof (uint8_t));
        break;

    case NSAE_CMD_KB_OVERFLOW:
    case NSAE_CMD_KB_CAPS:
    case NSAE_CMD_KB_CURSOR:
    case NSAE_CMD_KB_DATA:
    case NSAE_CMD_KB_INTERUPT:
        nsae_ipc_recieve_block ((uint8_t *)&kb_state, sizeof (uint8_t));
        break;

    case NSAE_CMD_RAM_READ:
        nsae_ipc_recieve_block ((uint8_t *)&abs_addr, sizeof (uint32_t));
        break;

    case NSAE_CMD_RAM_WRITE:
        nsae_ipc_recieve_block ((uint8_t *)&abs_addr, sizeof (uint32_t));
        nsae_ipc_recieve_block ((uint8_t *)&data, sizeof (uint8_t));
        break;

    case NSAE_CMD_MMU_LOAD:
        nsae_ipc_recieve_block ((uint8_t *)&mmu_slot, sizeof (uint8_t));
        nsae_ipc_recieve_block ((uint8_t *)&mmu_page, sizeof (uint8_t));
        break;

    case NSAE_CMD_MMU_READ:
        nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
        break;

    case NSAE_CMD_MMU_WRITE:
        nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
        nsae_ipc_recieve_block ((uint8_t *)&data, sizeof (uint8_t));
        break;
    }

    /* validate ranges */
    if (fd_num >= FD_CNT)
    {
        log_error ("nsae: server: fd_num out of range 0-1 -- %u\n", fd_num);
        free (file);
        return 1;
    }

    if (fd_side >= FD_SIDES)
    {
        log_error ("nsae: server: fd_side out of range 0-1 -- %u\n", fd_num);
        free (file);
        return 1;
    }

    if (fd_track >= FD_TRACKS)
    {
        log_error ("nsae: server: fd_track out of range 0-%u -- %u\n", 
                FD_TRACKS, fd_num);
        free (file);
        return 1;
    }

    if (fd_sector >= FD_SECTORS)
    {
        log_error ("nsae: server: fd_sector out of range 0-%u -- %u\n", 
                FD_SECTORS, fd_num);
        free (file);
        return 1;
    }

    if (kb_state >= 2)
    {
        log_error ("nsae: server: kb_state out of range 0-1 -- %u\n", 
                kb_state);
        free (file);
        return 1;
    }

    if (mmu_page >= MMU_PAGE_CNT)
    {
        log_error ("nsae: server: mmu_page out of range 0-%u -- %u\n", 
                MMU_PAGE_CNT, mmu_page);
        free (file);
        return 1;
    }

    if (mmu_slot >= MMU_SLOT_CNT)
    {
        log_error ("nsae: server: mmu_slot out of range 0-%u -- %u\n", 
                MMU_SLOT_CNT, mmu_slot);
        free (file);
        return 1;
    }

    if (abs_addr >= RAM_SIZE)
    {
        log_error ("nsae: server: abs_addr out of range 0-%u -- %u\n", 
                RAM_SIZE, abs_addr);
        free (file);
        return 1;
    }

    /* execute logic */
    switch (cmd)
    {

    /* emulator */
    case NSAE_CMD_EXIT:
        log_verbose ("nsae: server: exit\n");
        self->exit = true;
        break;

    case NSAE_CMD_RESTART:
        log_verbose ("nsae: server: restart\n");
        break;

    case NSAE_CMD_PAUSE:
        log_verbose ("nsae: server: pause\n");
        self->pause = true;
        break;

    case NSAE_CMD_CONTINUE:
        log_verbose ("nsae: server: continue\n");
        self->pause = false;
        break;

    case NSAE_CMD_BREAKPOINT:
        log_verbose ("nsae: server: breakpiont %04x\n", addr);
        break;

    case NSAE_CMD_STEP: 
        log_verbose ("nsae: server: step\n");
        self->step = true;
        self->step_pulse = true; 
        break;

    case NSAE_CMD_RUN:
        log_verbose ("nsae: server: run\n");
        self->step = false;
        break;

    case NSAE_CMD_STATUS:
        log_verbose ("nsae: server: status\n");
        log_info ("status: %d %d\n", self->pause, self->exit);
        break;


    /* floppy */
    case NSAE_CMD_FD_EJECT:
        log_verbose ("nsae: server: fd_eject %d\n", fd_num);
        fdc_eject (&self->adv.fdc, fd_num);
        break;

    case NSAE_CMD_FD_LOAD:
        log_verbose ("nsae: server: fd_load %d %s\n", fd_num, file);
        fdc_load_disk (&self->adv.fdc, fd_num, file);
        break;

    case NSAE_CMD_FD_SAVE:
        log_verbose ("nsae: server: fd_save %d %s\n", fd_num, file);
        fdc_save_disk (&self->adv.fdc, fd_num, file);
        break;

    case NSAE_CMD_FD_BLK_READ:
        log_verbose ("nsae: server: fd_blkdread %d %d %d %d\n", 
                fd_num, fd_side, fd_track, fd_sector);
        break;

    case NSAE_CMD_FD_STATUS:
        log_verbose ("nsae: server: fd_status %d\n", fd_num);
        break;


    /* harddisk */
    case NSAE_CMD_HD_EJECT:
        log_verbose ("nsae: server: hd_eject\n");
        break;

    case NSAE_CMD_HD_LOAD:
        log_verbose ("nsae: server: hd_load %s\n", file);
        break;

    case NSAE_CMD_HD_SAVE:
        log_verbose ("nsae: server: hd_save %s\n", file);
        break;

    case NSAE_CMD_HD_STATUS:
        log_verbose ("nsae: server: hd_status\n");
        break;


    /* keyboard */
    case NSAE_CMD_KB_PUSH:
        log_verbose ("nsae: server: kb_push %02x\n", kb_keycode);
        break;

    case NSAE_CMD_KB_POP:
        log_verbose ("nsae: server: kb_pop\n");
        break;

    case NSAE_CMD_KB_OVERFLOW:
        log_verbose ("nsae: server: kb_overflow %d\n", kb_state);
        break;

    case NSAE_CMD_KB_CAPS:
        log_verbose ("nsae: server: kb_caps %d\n", kb_state);
        break;

    case NSAE_CMD_KB_CURSOR:
        log_verbose ("nsae: server: kb_cursor %d\n", kb_state);
        break;

    case NSAE_CMD_KB_DATA:
        log_verbose ("nsae: server: kb_data %d\n", kb_state);
        break;

    case NSAE_CMD_KB_INTERUPT:
        log_verbose ("nsae: server: kb_interupt %d\n", kb_state);
        break;

    case NSAE_CMD_KB_STATUS:
        log_verbose ("nsae: server: kb_status\n");
        break;


    /* io boards */
    case NSAE_CMD_IO_STATUS:
        log_verbose ("nsae: server: io_status\n");
        break;


    /* crt display */
    case NSAE_CMD_CRT_STATUS:
        log_verbose ("nsae: server: crt_status\n");
        break;


    /* system advantage */
    case NSAE_CMD_ADV_STATUS:
        log_verbose ("nsae: server: adv_status\n");
        break;


    /* cpu z80 */
    case NSAE_CMD_CPU_STATUS:
        log_verbose ("nsae: server: cpu_status\n");
        break;


    /* raw ram */
    case NSAE_CMD_RAM_READ:
        log_verbose ("nsae: server: ram_read %05x\n", abs_addr);
        break;

    case NSAE_CMD_RAM_WRITE:
        log_verbose ("nsae: server: ram_write %05x %02x\n", abs_addr, data);
        break;


    /* prom */
    case NSAE_CMD_PROM_LOAD:
        log_verbose ("nsae: server: prom_load %s\n", file);
        break;


    /* memory multipliexer */
    case NSAE_CMD_MMU_READ:
        log_verbose ("nsae: server: mmu_read %04x\n", addr);
        break;

    case NSAE_CMD_MMU_WRITE:
        log_verbose ("nsae: server: mmu_write %04x %02x\n", addr, data);
        break;

    case NSAE_CMD_MMU_LOAD:
        log_verbose ("nsae: server: mmu_load %1x %1x\n", mmu_slot, mmu_page);
        break;

    case NSAE_CMD_MMU_STATUS:
        log_verbose ("nsae: server: mmu_status\n");
        break;
    }

    free (file);
    return 0;
}



/* end of file */
