
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

#include <z80emu.h>

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
    size_t file_len = 0;
    uint32_t abs_address = 0;

    /* poll packet_size */
    rc = nsae_ipc_recieve (&packet_size, sizeof (size_t));
    if (rc <= 0) return 0; /* non blocking */

    //g_log_categories[LC_SERVER] = LOG_DEBUG;
    log_debug ("server_handle_ipc.nsae_ipc_recieve.rc = %d\n", rc);

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

    if (packet_size > sizeof (nsae_packet_t))
    {
        file_len = packet_size - sizeof (nsae_packet_t);
        file = calloc (file_len + 1, sizeof (char));
        assert (file != NULL);
        memcpy (file, packet->buf, file_len);
    }

    /* execute logic */
    switch (packet->cmd)
    {
    /* emulator */
    case CMD_QUIT:
        log_verbose ("nsae: server: quit\n");
        self->exit = true;
        break;

    case CMD_RUN:
        log_verbose ("nsae: server: run\n");
        self->step = false;
        self->pause = false;
        self->resuming = true;
        break;

    case CMD_PAUSE:
        log_verbose ("nsae: server: pause\n");
        self->pause = true;
        break;

    case CMD_STEP:
        log_verbose ("nsae: server: step\n");
        self->step = true;
        self->pause = false;
        self->resuming = true;
        break;

    case CMD_NEXT:
        log_verbose ("nsae: server: next\n");
        break;

    case CMD_INFO:
        /* {{{ */
        log_verbose ("nsae: server: info ");
        switch (packet->mode)
        {
        case MODE_BR:
            log_verbose ("breakpoint\n");
            br_list (&self->br);
            break;

        case MODE_WP:
            log_verbose ("watchpoint\n");
            wp_list (&self->wp);
            break;

        case MODE_ADV:
            log_verbose ("advantage\n");
            adv_status (&self->adv);
            break;

        case MODE_CPU:
            log_verbose ("cpu\n");
            cpu_status (&self->adv.cpu, &self->adv.mmu);
            break;

        case MODE_CRT:
            log_verbose ("display\n");
            crt_status (&self->adv.crt);
            break;

        case MODE_FDC:
            log_verbose ("floppy\n");
            fdc_status (&self->adv.fdc);
            break;

        case MODE_HDC:
            log_verbose ("harddrive\n");
            log_error ("UNIMPLIMENTED\n");
            break;

        case MODE_IO:
            log_verbose ("io\n");
            log_error ("UNIMPLIMENTED\n");
            break;

        case MODE_PROM:
            log_verbose ("prom\n");
            log_error ("UNIMPLIMENTED\n");
            break;

        case MODE_KB:
            log_verbose ("keyboard\n");
            kb_status (&self->adv.kb);
            break;

        case MODE_MMU:
            log_verbose ("mmu\n");
            mmu_status (&self->adv.mmu);
            break;

        case MODE_NSAE:
            log_verbose ("nsae\n");
            nsae_status (self);
            break;

        case MODE_LOG:
            log_verbose ("log\n");
            log_error ("UNIMPLIMENTED\n");
            break;
        }
        /* }}} */
        break;

    case CMD_DELETE:
        /* {{{ */
        log_verbose ("nsae: server: delete ");
        switch (packet->mode)
        {
        case MODE_WP:
            log_verbose ("watchpoint %d\n", packet->v_index);
            wp_remove (&self->wp, packet->v_index);
            break;

        case MODE_BR:
            log_verbose ("breakpoint %d\n", packet->v_index);
            br_remove (&self->br, packet->v_index);
            break;

        case MODE_KB:
            log_verbose ("keyboard %d\n", packet->v_index);
            kb_pop (&self->adv.kb);
            break;
        }
        /* }}} */
        break;

    case CMD_SET:
        log_verbose ("nsae: server: set "); 
        switch (packet->var)
        {
        case VAR_BR_APPEND:
            log_verbose ("breakpoint.append %04x\n", packet->v_data32);
            br_add (&self->br, packet->v_data32);
            break;

        case VAR_ADV_KBMI:
            log_verbose ("advantage.kbmi %1d\n", packet->v_data32);
            self->adv.kb_mi = (packet->v_data32 > 0);
            break;

        case VAR_ADV_KBNMI:
            log_verbose ("advantage.kbnmi %1d\n", packet->v_data32);
            self->adv.kb_nmi = (packet->v_data32 > 0);
            break;

        case VAR_ADV_CRTMI:
            log_verbose ("advantage.crtmi %1d\n", packet->v_data32);
            self->adv.crt_mi = (packet->v_data32 > 0);
            break;

        case VAR_ADV_INTERUPT:
            log_verbose ("advantage.interupt %1d\n", packet->v_data32);
            self->adv.hw_interupt = (packet->v_data32 > 0);
            break;

        case VAR_ADV_CMDACK:
            log_verbose ("advantage.cmdack %1d\n", packet->v_data32);
            self->adv.cmd_ack = (packet->v_data32 > 0);
            break;

        case VAR_PORT_OUT:
            log_verbose ("port.out %02x %02x\n", packet->v_port, packet->v_data32);
            adv_out (&self->adv, packet->v_port, packet->v_data32, 0x0000);
            break;

        case VAR_PORT_IN:
            log_verbose ("port.in %02x\n", packet->v_port);
            adv_in (&self->adv, packet->v_port, 0x0000);
            break;

        case VAR_CPU_A:
            log_verbose ("cpu.a %02x\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_A] = (packet->v_data32 & 0xff);
            break;

        case VAR_CPU_BC:
            log_verbose ("cpu.bc %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_BC] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_DE:
            log_verbose ("cpu.de %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_DE] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_HL:
            log_verbose ("cpu.hl %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_HL] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_PC:
            log_verbose ("cpu.pc %04x %x\n", packet->v_data32);
            self->adv.cpu.state.pc = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_SP:
            log_verbose ("cpu.sp %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_SP] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_IX:
            log_verbose ("cpu.ix %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_IX] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_IY:
            log_verbose ("cpu.iy %04x\n", packet->v_data32);
            self->adv.cpu.state.registers.word[Z80_IY] = (packet->v_data32 & 0xffff);
            break;

        case VAR_CPU_I:
            log_verbose ("cpu.i %x\n", packet->v_data32);
            self->adv.cpu.state.i = (packet->v_data32 & 0x1);
            break;

        case VAR_CPU_R:
            log_verbose ("cpu.r %x\n", packet->v_data32);
            self->adv.cpu.state.r = (packet->v_data32 & 0x1);
            break;

        case VAR_CPU_IFF1:
            log_verbose ("cpu.iff1 %x\n", packet->v_data32);
            self->adv.cpu.state.iff1 = (packet->v_data32 & 0x1);
            break;

        case VAR_CPU_IFF2:
            log_verbose ("cpu.iff2 %x\n", packet->v_data32);
            self->adv.cpu.state.iff2 = (packet->v_data32 & 0x1);
            break;

        case VAR_CPU_IM:
            log_verbose ("cpu.im %x\n", packet->v_data32);
            self->adv.cpu.state.im = (packet->v_data32 & 0x1);
            break;

        case VAR_CPU_EXX:
            log_verbose ("cpu.exx %1d\n", packet->v_data32);
            break;

        case VAR_CPU_HALT:
            log_verbose ("cpu.halt %1d\n", packet->v_data32);
            break;

        case VAR_CPU_S_FLAG:
            log_verbose ("cpu.flag.s %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_S_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_S_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CPU_Z_FLAG:
            log_verbose ("cpu.flag.z %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_Z_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_Z_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CPU_H_FLAG:
            log_verbose ("cpu.flag.h %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_H_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_H_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CPU_P_FLAG:
        case VAR_CPU_V_FLAG:
            log_verbose ("cpu.flag.pv %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_PV_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_PV_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CPU_N_FLAG:
            log_verbose ("cpu.flag.n %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_N_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_N_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CPU_C_FLAG:
            log_verbose ("cpu.flag.c %1d\n", packet->v_data32);
            self->adv.cpu.state.registers.byte[Z80_F] &= Z80_C_FLAG;
            self->adv.cpu.state.registers.byte[Z80_F] |= Z80_C_FLAG * (packet->v_data32 > 0);
            break;

        case VAR_CRT_BACKGROUND:
            log_verbose ("crt.background %06x\n", packet->v_data32);
            self->adv.crt.background.x = (float)(packet->v_data32 & 0x00ff0000 >> 16) / 255;
            self->adv.crt.background.y = (float)(packet->v_data32 & 0x0000ff00 >> 8) / 255;
            self->adv.crt.background.z = (float)(packet->v_data32 & 0x000000ff) / 255;
            break;

        case VAR_CRT_FOREGROUND:
            log_verbose ("crt.foreground %06x\n", packet->v_data32);
            self->adv.crt.foreground.x = (float)(packet->v_data32 & 0x00ff0000 >> 16) / 255;
            self->adv.crt.foreground.y = (float)(packet->v_data32 & 0x0000ff00 >> 8) / 255;
            self->adv.crt.foreground.z = (float)(packet->v_data32 & 0x000000ff) / 255;
            break;

        case VAR_CRT_BLANK:
            log_verbose ("crt.blank %1d\n", packet->v_data32);
            self->adv.crt.blank = (packet->v_data32 > 0);
            break;

        case VAR_CRT_VSYNC:
            log_verbose ("crt.vsync %1d\n", packet->v_data32);
            self->adv.crt.vrefresh = (packet->v_data32 > 0);
            break;

        case VAR_CRT_INVERTED:
            log_verbose ("crt.inverted %02x\n", packet->v_data32);
            self->adv.crt.inverted = (packet->v_data32 > 0);
            break;

        case VAR_CRT_SCROLL:
            log_verbose ("crt.scroll %02x\n", packet->v_data32);
            self->adv.crt.scroll_reg = packet->v_data32;
            break;

        case VAR_FDC_DISK:
            log_verbose ("fdc.disk %1d\n", packet->v_data32);
            self->adv.fdc.disk = packet->v_data32 & 1;
            break;

        case VAR_FDC_SIDE:
            log_verbose ("fdc.side %1d\n", packet->v_data32);
            self->adv.fdc.side = packet->v_data32 & 1;
            break;

        case VAR_FDC_TRACK:
            log_verbose ("fdc.track %02x\n", packet->v_data32);
            self->adv.fdc.track = packet->v_data32;
            break;

        case VAR_FDC_POWERED:
            log_verbose ("fdc.powered %1d\n", packet->v_data32);
            self->adv.fdc.powered = (packet->v_data32 > 0);
            break;

        case VAR_FDC_TRACKZERO:
            log_verbose ("fdc.trackzero %1d\n", packet->v_data32);
            self->adv.fdc.track_zero = (packet->v_data32 > 0);
            break;

        case VAR_FDC_SECTORMARK:
            log_verbose ("fdc.sectormark %1d\n", packet->v_data32);
            self->adv.fdc.sector_mark = (packet->v_data32 > 0);
            break;

        case VAR_FDC_EJECT_A:
            log_verbose ("fdc.eject.a %1d\n", packet->v_data32);
            fdc_eject (&self->adv.fdc, 0);
            break;

        case VAR_FDC_EJECT_B:
            log_verbose ("fdc.eject.b %1d\n", packet->v_data32);
            fdc_eject (&self->adv.fdc, 1);
            break;

        case VAR_FDC_SECTOR_A:
            log_verbose ("fdc.sector.a %1x\n", packet->v_data32);
            self->adv.fdc.sector[0] = packet->v_data32;
            break;

        case VAR_FDC_SECTOR_B:
            log_verbose ("fdc.sector.b %1x\n", packet->v_data32);
            self->adv.fdc.sector[1] = packet->v_data32;
            break;

        case VAR_HDC_EJECT:
            log_verbose ("hdc.eject %1d\n", packet->v_data32);
            break;

        case VAR_KB_REPEAT:
            log_verbose ("kb.repeat %1d\n", packet->v_data32);
            self->adv.kb.autorepeat = (packet->v_data32 > 0);
            break;

        case VAR_KB_CAPSLOCK:
            log_verbose ("kb.capslock %1d\n", packet->v_data32);
            self->adv.kb.caps_lock = (packet->v_data32 > 0);
            break;

        case VAR_KB_CURSORLOCK:
            log_verbose ("kb.cursorlock %1d\n", packet->v_data32);
            self->adv.kb.cursor_lock = (packet->v_data32 > 0);
            break;

        case VAR_KB_OVERFLOW:
            log_verbose ("kb.overflow %1d\n", packet->v_data32);
            self->adv.kb.overflow = (packet->v_data32 > 0);
            break;

        case VAR_KB_DATAFLAG:
            log_verbose ("kb.dataflag %1d\n", packet->v_data32);
            self->adv.kb.data_flag = (packet->v_data32 > 0);
            break;

        case VAR_KB_INTERRUPT:
            log_verbose ("kb.interrupt %1d\n", packet->v_data32);
            self->adv.kb.hw_interrupt = (packet->v_data32 > 0);
            break;

        case VAR_KB_PRESS:
            log_verbose ("kb.press %02x\n", packet->v_data32);
            kb_push (&self->adv.kb, packet->v_data32);
            break;

        case VAR_MMU_SLOT0:
            log_verbose ("mmu.slot0 %02x\n", packet->v_data32);
            mmu_load_page (&self->adv.mmu, 0, packet->v_data32); 
            break;

        case VAR_MMU_SLOT1:
            log_verbose ("mmu.slot1 %02x\n", packet->v_data32);
            mmu_load_page (&self->adv.mmu, 1, packet->v_data32); 
            break;

        case VAR_MMU_SLOT2:
            log_verbose ("mmu.slot2 %02x\n", packet->v_data32);
            mmu_load_page (&self->adv.mmu, 2, packet->v_data32); 
            break;

        case VAR_MMU_SLOT3:
            log_verbose ("mmu.slot3 %02x\n", packet->v_data32);
            mmu_load_page (&self->adv.mmu, 3, packet->v_data32); 
            break;

        case VAR_LOG_CRT:
            log_verbose ("log.crt %1d\n", packet->v_data32);
            g_log_categories[LC_CRT] = packet->v_data32;
            break;

        case VAR_LOG_CPU:
            log_verbose ("log.cpu %1d\n", packet->v_data32);
            g_log_categories[LC_CPU] = packet->v_data32;
            break;

        case VAR_LOG_KB:
            log_verbose ("log.kb %1d\n", packet->v_data32);
            g_log_categories[LC_KB] = packet->v_data32;
            break;

        case VAR_LOG_RAM:
            log_verbose ("log.ram %1d\n", packet->v_data32);
            g_log_categories[LC_RAM] = packet->v_data32;
            break;

        case VAR_LOG_MMU:
            log_verbose ("log.mmu %1d\n", packet->v_data32);
            g_log_categories[LC_MMU] = packet->v_data32;
            break;

        case VAR_LOG_FDC:
            log_verbose ("log.fdc %1d\n", packet->v_data32);
            g_log_categories[LC_FDC] = packet->v_data32;
            break;

        case VAR_LOG_HDC:
            log_verbose ("log.hdc %1d\n", packet->v_data32);
            g_log_categories[LC_HDC] = packet->v_data32;
            break;

        case VAR_LOG_IO:
            log_verbose ("log.io %1d\n", packet->v_data32);
            g_log_categories[LC_IO] = packet->v_data32;
            g_log_categories[LC_SIO] = packet->v_data32;
            g_log_categories[LC_PIO] = packet->v_data32;
            break;

        case VAR_LOG_ADV:
            log_verbose ("log.adv %1d\n", packet->v_data32);
            g_log_categories[LC_ADV] = packet->v_data32;
            g_log_categories[LC_SPEAKER] = packet->v_data32;
            break;

        case VAR_LOG_NSAE:
            log_verbose ("log.nsae %1d\n", packet->v_data32);
            g_log_categories[LC_NSAE] = packet->v_data32;
            g_log_categories[LC_SERVER] = packet->v_data32;
            g_log_categories[LC_BREAKPOINTS] = packet->v_data32;
            g_log_categories[LC_WATCHPOINTS] = packet->v_data32;
            break;

        case VAR_LOG_ALL:
            log_verbose ("log.all %1d\n", packet->v_data32);
            memset (g_log_categories, packet->v_data32, g_log_categories_count);
            break;

        case VAR_LOG_OUTPUT_FILE:
            log_verbose ("log.outputfile %s\n", file);
            fclose (g_log_file_stream);
            g_log_file_stream = fopen (file, "a+");
            if (g_log_file_stream == NULL)
            {
                log_error ("nsae: server: cannot open -- %s\n", file);
            }
            break;
        }
        break;

    case CMD_LOAD:
        log_verbose ("nsae: server: load ");
        switch (packet->mode)
        {
        case MODE_FDC:
            log_verbose ("floppy %d %s\n", packet->v_fddrive, file);
            fdc_load_disk (&self->adv.fdc, packet->v_fddrive, file);
            break;

        case MODE_HDC:
            log_verbose ("harddisk\n");
            break;

        case MODE_RAM:
            log_verbose ("ram\n");
            ram_load_disk (&self->adv.ram, packet->v_addr32, file, -1);
            break;

        case MODE_MMU:
            log_verbose ("mmu\n");
            abs_address = mmu_decode (&self->adv.mmu, packet->v_addr32);
            ram_load_disk (&self->adv.ram, abs_address, file, -1);
            break;

        case MODE_PROM:
            log_verbose ("prom %s\n", file);
            ram_load_disk (&self->adv.ram, RAM_BASE_PROM, file, RAM_PROM_SIZE);
            break;
        }
        break;

    case CMD_SAVE:
        log_verbose ("nsae: server: save ");
        switch (packet->mode)
        {
        case MODE_FDC:
            log_verbose ("floppy %d %s\n", packet->v_fddrive, file);
            fdc_save_disk (&self->adv.fdc, packet->v_fddrive, file);
            break;

        case MODE_HDC:
            log_verbose ("harddisk\n");
            break;

        case MODE_RAM:
            log_verbose ("ram\n");
            ram_save_disk (&self->adv.ram, packet->v_addr32, packet->v_count, file);
            break;

        case MODE_MMU:
            log_verbose ("mmu\n");
            abs_address = mmu_decode (&self->adv.mmu, packet->v_addr32);
            ram_save_disk (&self->adv.ram, abs_address, packet->v_count, file);
            break;
        }
        break;

    case CMD_READ:
        log_verbose ("nsae: server: read ");
        switch (packet->mode)
        {
        case MODE_RAM:
            log_verbose ("ram %05zu %05zu\n", packet->v_addr32, packet->v_count);
            ram_inspect (&self->adv.ram, packet->v_addr32, packet->v_count);
            break;

        case MODE_MMU:
            log_verbose ("mmu %04zu %05zu\n", packet->v_addr32, packet->v_count);
            abs_address = mmu_decode (&self->adv.mmu, packet->v_addr32);
            ram_inspect (&self->adv.ram, abs_address, packet->v_count);
            break;
        }
        break;

    case CMD_WRITE:
        log_verbose ("nsae: server: write ");
        switch (packet->mode)
        {
        case MODE_RAM:
            log_verbose ("ram %05zu %02u\n", packet->v_addr32, packet->v_data32);
            ram_write (&self->adv.ram, packet->v_addr32, packet->v_data32);
            break;

        case MODE_MMU:
            log_verbose ("mmu %05zu %02u\n", packet->v_addr32, packet->v_data32);
            mmu_write (&self->adv.mmu, packet->v_addr32, packet->v_data32);
            break;
        }
        break;
    }

    return 0;
}

/* end of file */
