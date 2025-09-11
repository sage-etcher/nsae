
#define LOG_CATEGORY LC_SERVER
#include "server.h"

#include "adv.h"
#include "fdc.h"
#include "mmu.h"
#include "nsae.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "nslog.h"
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
    int rc = 0;
    uint32_t packet_size = 0;
    nsae_packet_t *packet = NULL;
    char *file = NULL;

    /* poll packet_size */
    rc = nsae_ipc_recieve (&packet_size, sizeof (uint32_t));
    if (rc == 0) return 0; /* non blocking */

    /* enforce minimum packet size */
    if (packet_size < sizeof (nsae_packet_t))
    {
        log_error ("nsae: server: packet_size cannot be less than %zu, but got %zu\n", 
                sizeof (nsae_packet_t), packet_size);
        return 1;
    }

    /* allocate packet */
    packet = malloc (packet_size);
    if (packet == NULL) return 1;

    /* get packet */
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
            log_error ("nsae: server: packet_size buflen mismatch %zu %zu\n",
                    packet_size, packet->v_buflen + sizeof (nsae_packet_t));
            free (packet);
            return 1;
        }

        /* copy the filename */
        file = malloc (packet->v_buflen + 1);
        if (file == NULL)
        {
            free (packet);
            return 1;
        }
        memcpy (file, &packet->buf, packet->v_buflen);
        file[packet->v_buflen] = '\0';

        break;
    }

    /* execute logic */
    switch (packet->cmd)
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
        log_verbose ("nsae: server: breakpoint set %04x\n", packet->v_addr16);
        br_add (&self->br, packet->v_addr16);
        break;

    case NSAE_CMD_BRKPNT_REMOVE:
        log_verbose ("nsae: server: breakpoint remove %04x\n", 
                packet->v_addr16);
        br_remove (&self->br, packet->v_addr16);
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
        log_verbose ("nsae: server: log_cpu %d\n", packet->v_state);
        log_set_cat (LC_CPU, packet->v_state);
        break;

    case NSAE_CMD_LOG_MMU:
        log_verbose ("nsae: server: log_mmu %d\n", packet->v_state);
        log_set_cat (LC_MMU, packet->v_state);
        break;

    case NSAE_CMD_LOG_RAM:
        log_verbose ("nsae: server: log_ram %d\n", packet->v_state);
        log_set_cat (LC_RAM, packet->v_state);
        break;

    case NSAE_CMD_LOG_FDC:
        log_verbose ("nsae: server: log_fdc %d\n", packet->v_state);
        log_set_cat (LC_FDC, packet->v_state);
        break;

    case NSAE_CMD_LOG_CRT:
        log_verbose ("nsae: server: log_crt %d\n", packet->v_state);
        log_set_cat (LC_CRT, packet->v_state);
        break;

    case NSAE_CMD_LOG_KB:
        log_verbose ("nsae: server: log_kb %d\n", packet->v_state);
        log_set_cat (LC_KB, packet->v_state);
        break;

    case NSAE_CMD_LOG_MOBO:
        log_verbose ("nsae: server: log_mobo %d\n", packet->v_state);
        log_set_cat (LC_ADV, packet->v_state);
        break;

    case NSAE_CMD_LOG_TERSE:
        log_verbose ("nsae: server: log_terse\n");
        log_set (LOG_INFO);
        break;

    case NSAE_CMD_LOG_VERBOSE:
        log_verbose ("nsae: server: log_verbose\n");
        log_set (LOG_VERBOSE);
        break;

    case NSAE_CMD_LOG_DEBUG:
        log_verbose ("nsae: server: log_debug\n");
        log_set (LOG_DEBUG);
        break;

    case NSAE_CMD_LOG_OUTPUT:
        log_verbose ("nsae: server: log_output %s\n", file);
        g_log_file_stream = fopen (file, "a+");
        if (g_log_file_stream == NULL)
        {
            log_error ("nsae: server: cannot open -- %s\n", file);
        }
        break;

    /* floppy */
    case NSAE_CMD_FD_EJECT:
        log_verbose ("nsae: server: fd_eject %d\n", packet->v_fd_num);
        fdc_eject (&self->adv.fdc, packet->v_fd_num);
        break;

    case NSAE_CMD_FD_LOAD:
        log_verbose ("nsae: server: fd_load %d %s\n", packet->v_fd_num, file);
        fdc_load_disk (&self->adv.fdc, packet->v_fd_num, file);
        break;

    case NSAE_CMD_FD_SAVE:
        log_verbose ("nsae: server: fd_save %d %s\n", packet->v_fd_num, file);
        fdc_save_disk (&self->adv.fdc, packet->v_fd_num, file);
        break;

    case NSAE_CMD_FD_BLK_READ:
        log_verbose ("nsae: server: fd_blkdread %d %d %d %d\n", 
                packet->v_fd_num, 
                packet->v_fd_side, 
                packet->v_fd_track, 
                packet->v_fd_sector);
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
        log_verbose ("nsae: server: kb_push %02x\n", packet->v_keycode);
        kb_push (&self->adv.kb, packet->v_keycode);
        break;

    case NSAE_CMD_KB_POP:
        log_verbose ("nsae: server: kb_pop\n");
        kb_pop (&self->adv.kb);
        break;

    case NSAE_CMD_KB_OVERFLOW:
        log_verbose ("nsae: server: kb_overflow %d\n", packet->v_state);
        self->adv.kb.overflow = packet->v_state & 1;
        break;

    case NSAE_CMD_KB_CAPS:
        log_verbose ("nsae: server: kb_caps %d\n", packet->v_state);
        self->adv.kb.caps_lock = packet->v_state & 1;
        break;

    case NSAE_CMD_KB_CURSOR:
        log_verbose ("nsae: server: kb_cursor %d\n", packet->v_state);
        self->adv.kb.cursor_lock = packet->v_state & 1;
        break;

    case NSAE_CMD_KB_DATA:
        log_verbose ("nsae: server: kb_data %d\n", packet->v_state);
        self->adv.kb.data_flag = packet->v_state & 1;
        break;

    case NSAE_CMD_KB_INTERUPT:
        log_verbose ("nsae: server: kb_interupt %d\n", packet->v_state);
        self->adv.kb.reset = packet->v_state & 1;
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
        log_verbose ("nsae: server: adv_out %02x %02x\n", 
                packet->v_port, packet->v_data);
        adv_out (&self->adv, packet->v_port, packet->v_data, 0x0000);
        break;

    case NSAE_CMD_ADV_IN:
        log_verbose ("nsae: server: adv_in %02x\n", packet->v_port);
        log_info ("port (%02x): %02x\n", packet->v_port, 
                adv_in (&self->adv, packet->v_port, 0x00000));
        break;


    /* cpu z80 */
    case NSAE_CMD_CPU_STATUS:
        log_verbose ("nsae: server: cpu_status\n");
        cpu_status (&self->adv.cpu, &self->adv.mmu);
        break;


    /* raw ram */
    case NSAE_CMD_RAM_READ:
        log_verbose ("nsae: server: ram_read %05x %zu\n", 
                packet->v_addr32, packet->v_span32);
        ram_inspect (&self->adv.ram, packet->v_addr32, packet->v_span32);
        break;

    case NSAE_CMD_RAM_WRITE:
        log_verbose ("nsae: server: ram_write %05x %02x\n", 
                packet->v_addr32, packet->v_data);
        ram_write (&self->adv.ram, packet->v_addr32, packet->v_data);
        break;


    /* prom */
    case NSAE_CMD_PROM_LOAD:
        log_verbose ("nsae: server: prom_load %s\n", file);
        ram_load_prom_from_file (&self->adv.ram, file);
        break;


    /* memory multipliexer */
    case NSAE_CMD_MMU_READ:
        log_verbose ("nsae: server: mmu_read %04x %zu\n", 
                    packet->v_addr16, packet->v_span16);
        ram_inspect (
                &self->adv.ram, 
                mmu_decode (&self->adv.mmu, packet->v_addr32),
                packet->v_span16);
        break;

    case NSAE_CMD_MMU_WRITE:
        log_verbose ("nsae: server: mmu_write %04x %02x\n", 
                packet->v_addr16, packet->v_data);
        mmu_write (&self->adv.mmu, packet->v_addr16, packet->v_data);
        break;

    case NSAE_CMD_MMU_LOAD:
        log_verbose ("nsae: server: mmu_load %1x %1x\n", 
                packet->v_slot, packet->v_page);
        mmu_load_page (&self->adv.mmu, packet->v_slot, packet->v_page);
        break;

    case NSAE_CMD_MMU_STATUS:
        log_verbose ("nsae: server: mmu_status\n");
        mmu_status (&self->adv.mmu);
        break;
    }

    free (file);
    free (packet);
    return 0;
}



/* end of file */
