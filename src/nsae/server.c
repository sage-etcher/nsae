
#include "server.h"

#include "adv.h"
#include "fdc.h"
#include "log.h"
#include "mmu.h"
#include "nsae.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "nslog.h"
#include "ram.h"

#include <assert.h>
#include <ctype.h>
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
    uint8_t log_state = 0x00;
    uint8_t mmu_page = 0x00;
    uint8_t mmu_slot = 0x00;
    uint8_t data = 0x00;
    uint16_t addr = 0x0000;
    uint32_t abs_addr = 0x00000000;
    char *file = NULL;
    uint8_t port = 0x00;
    size_t n = 0;

    int rc = nsae_ipc_recieve (&cmd, sizeof (uint8_t));
    if (rc == 0) return 0; /* non blocking */

    /* get parameters */
    switch (cmd)
    {
    case NSAE_CMD_BRKPNT_SET:
    case NSAE_CMD_BRKPNT_REMOVE:
        nsae_ipc_recieve_block ((uint8_t *)&addr, sizeof (uint16_t));
        break;

    case NSAE_CMD_FD_EJECT:
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
    case NSAE_CMD_LOG_OUTPUT:
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

    case NSAE_CMD_LOG_CPU:
    case NSAE_CMD_LOG_MMU:
    case NSAE_CMD_LOG_RAM:
    case NSAE_CMD_LOG_FDC:
    case NSAE_CMD_LOG_CRT:
    case NSAE_CMD_LOG_KB:
    case NSAE_CMD_LOG_MOBO:
    case NSAE_CMD_LOG_VERBOSE:
    case NSAE_CMD_LOG_DEBUG:
        nsae_ipc_recieve_block ((uint8_t *)&log_state, sizeof (uint8_t));
        break;

    case NSAE_CMD_ADV_IN:
        nsae_ipc_recieve_block ((uint8_t *)&port, sizeof (uint8_t));
        break;

    case NSAE_CMD_ADV_OUT:
        nsae_ipc_recieve_block ((uint8_t *)&port, sizeof (uint8_t));
        nsae_ipc_recieve_block ((uint8_t *)&data, sizeof (uint8_t));
        break;

    case NSAE_CMD_RAM_READ:
        nsae_ipc_recieve_block ((uint8_t *)&abs_addr, sizeof (uint32_t));
        nsae_ipc_recieve_block ((uint8_t *)&n, sizeof (size_t));
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
        nsae_ipc_recieve_block ((uint8_t *)&n, sizeof (size_t));
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

    if (log_state >= 2)
    {
        log_error ("nsae: server: log_state out of range 0-1 -- %u\n", 
                log_state);
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

    case NSAE_CMD_BRKPNT_SET:
        log_verbose ("nsae: server: breakpoint set %04x\n", addr);
        br_add (&self->br, addr);
        break;

    case NSAE_CMD_BRKPNT_REMOVE:
        log_verbose ("nsae: server: breakpoint remove %04x\n", addr);
        br_remove (&self->br, addr);
        break;

    case NSAE_CMD_BRKPNT_LIST:
        log_verbose ("nsae: server: breakpoint list\n");
        log_info ("breakpoint list:\n");
        for (size_t i = 0; i < self->br.cnt; i++)
        {
            log_info ("%3lu: %04x\n", i, self->br.m[i]);
        }
        break;

    case NSAE_CMD_STEP: 
        log_verbose ("nsae: server: step\n");
        self->step = true;
        self->pause = false;
        self->resuming = true;
        break;

    case NSAE_CMD_RUN:
        log_verbose ("nsae: server: run\n");
        self->step = false;
        self->pause = false;
        self->resuming = true;
        break;

    case NSAE_CMD_STATUS:
        log_verbose ("nsae: server: status\n");
        nsae_status (self);
        break;

    /* log */
    case NSAE_CMD_LOG_CPU:
        log_verbose ("nsae: server: log_cpu %d\n", log_state);
        g_log_cpu = log_state;
        break;

    case NSAE_CMD_LOG_MMU:
        log_verbose ("nsae: server: log_mmu %d\n", log_state);
        g_log_mmu = log_state;
        break;

    case NSAE_CMD_LOG_RAM:
        log_verbose ("nsae: server: log_ram %d\n", log_state);
        g_log_ram = log_state;
        break;

    case NSAE_CMD_LOG_FDC:
        log_verbose ("nsae: server: log_fdc %d\n", log_state);
        g_log_fdc= log_state;
        break;

    case NSAE_CMD_LOG_CRT:
        log_verbose ("nsae: server: log_crt %d\n", log_state);
        g_log_crt = log_state;
        break;

    case NSAE_CMD_LOG_KB:
        log_verbose ("nsae: server: log_kb %d\n", log_state);
        g_log_kb = log_state;
        break;

    case NSAE_CMD_LOG_MOBO:
        log_verbose ("nsae: server: log_mobo %d\n", log_state);
        g_log_mobo = log_state;
        break;

    case NSAE_CMD_LOG_VERBOSE:
        log_verbose ("nsae: server: log_verbose %d\n", log_state);
        g_log_verbose = log_state;
        break;

    case NSAE_CMD_LOG_DEBUG:
        log_verbose ("nsae: server: log_debug %d\n", log_state);
        g_log_debug = log_state;
        break;

    case NSAE_CMD_LOG_OUTPUT:
        log_verbose ("nsae: server: log_output %s\n", file);
        g_log_fp = fopen (file, "a+");
        if (g_log_fp == NULL)
        {
            log_error ("nsae: server: cannot open -- %s\n", file);
        }
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
        log_verbose ("nsae: server: fd_status\n");
        fdc_status (&self->adv.fdc);
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
        kb_push (&self->adv.kb, kb_keycode);
        break;

    case NSAE_CMD_KB_POP:
        log_verbose ("nsae: server: kb_pop\n");
        kb_pop (&self->adv.kb);
        break;

    case NSAE_CMD_KB_OVERFLOW:
        log_verbose ("nsae: server: kb_overflow %d\n", kb_state);
        self->adv.kb.overflow = kb_state;
        break;

    case NSAE_CMD_KB_CAPS:
        log_verbose ("nsae: server: kb_caps %d\n", kb_state);
        self->adv.kb.caps_lock = kb_state;
        break;

    case NSAE_CMD_KB_CURSOR:
        log_verbose ("nsae: server: kb_cursor %d\n", kb_state);
        self->adv.kb.cursor_lock = kb_state;
        break;

    case NSAE_CMD_KB_DATA:
        log_verbose ("nsae: server: kb_data %d\n", kb_state);
        self->adv.kb.data_flag = kb_state;
        break;

    case NSAE_CMD_KB_INTERUPT:
        log_verbose ("nsae: server: kb_interupt %d\n", kb_state);
        self->adv.kb.reset = kb_state;
        break;

    case NSAE_CMD_KB_STATUS:
        log_verbose ("nsae: server: kb_status\n");
        kb_status (&self->adv.kb);
        break;


    /* io boards */
    case NSAE_CMD_IO_STATUS:
        log_verbose ("nsae: server: io_status\n");
        break;


    /* crt display */
    case NSAE_CMD_CRT_STATUS:
        log_verbose ("nsae: server: crt_status\n");
        crt_status (&self->adv.crt);
        break;


    /* system advantage */
    case NSAE_CMD_ADV_STATUS:
        log_verbose ("nsae: server: adv_status\n");
        adv_status (&self->adv);
        break;

    case NSAE_CMD_ADV_OUT:
        log_verbose ("nsae: server: adv_out %02x %02x\n", port, data);
        adv_out (&self->adv, port, data, 0x0000);
        break;

    case NSAE_CMD_ADV_IN:
        log_verbose ("nsae: server: adv_in %02x\n", port);
        log_info ("port (%02x): %02x\n", port, adv_in (&self->adv, port, 0x00000));
        break;


    /* cpu z80 */
    case NSAE_CMD_CPU_STATUS:
        log_verbose ("nsae: server: cpu_status\n");
        cpu_status (&self->adv.cpu, &self->adv.mmu);
        break;


    /* raw ram */
    case NSAE_CMD_RAM_READ:
        log_verbose ("nsae: server: ram_read %05x %zu\n", abs_addr, n);
        ram_inspect (&self->adv.ram, abs_addr, n);
        break;

    case NSAE_CMD_RAM_WRITE:
        log_verbose ("nsae: server: ram_write %05x %02x\n", abs_addr, data);
        ram_write (&self->adv.ram, abs_addr, data);
        break;


    /* prom */
    case NSAE_CMD_PROM_LOAD:
        log_verbose ("nsae: server: prom_load %s\n", file);
        ram_load_prom_from_file (&self->adv.ram, file);
        break;


    /* memory multipliexer */
    case NSAE_CMD_MMU_READ:
        log_verbose ("nsae: server: mmu_read %04x %zu\n", addr, n);
        abs_addr = mmu_decode (&self->adv.mmu, addr);
        ram_inspect (&self->adv.ram, abs_addr, n);
        break;

    case NSAE_CMD_MMU_WRITE:
        log_verbose ("nsae: server: mmu_write %04x %02x\n", addr, data);
        mmu_write (&self->adv.mmu, addr, data);
        break;

    case NSAE_CMD_MMU_LOAD:
        log_verbose ("nsae: server: mmu_load %1x %1x\n", mmu_slot, mmu_page);
        mmu_load_page (&self->adv.mmu, mmu_slot, mmu_page);
        break;

    case NSAE_CMD_MMU_STATUS:
        log_verbose ("nsae: server: mmu_status\n");
        mmu_status (&self->adv.mmu);
        break;
    }

    free (file);
    return 0;
}



/* end of file */
