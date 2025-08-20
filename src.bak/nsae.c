
#include "nsae.h"

#include "advprom.h"
#include "cpmbasic_120.h"
#include "mmu.h"
#include "z80emu.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char *const LOGFILE = "nsae.log";
static FILE *s_log = NULL;

static void adv_set_memory_map (adv_t *self, uint8_t port, uint8_t data);
static void adv_fdc_register (adv_t *self, uint8_t data);

static uint8_t *adv_fd_data_ptr (adv_t *self);
static uint8_t adv_fd_read (adv_t *self);
static void adv_fd_write (adv_t *self, uint8_t data);

int
adv_init (adv_t *self)
{
    /* initialize the emulator */
    self->win_exit = 0;
    self->emu_paused = 0;

    self->win_width   = 640;
    self->win_height  = 480;
    self->win_max_fps = 60;

    /* initialize the cpu */
    Z80Reset (&self->cpu);

    /* initialize the mmu */
    self->mmu.slot_bit = 14; /* 16kb slots */

    mmu_init_page (&self->mmu, 0x00, 0xffff, 0x00000); /* 16k main */
    mmu_init_page (&self->mmu, 0x01, 0xffff, 0x04000); /* 16k main */
    mmu_init_page (&self->mmu, 0x02, 0xffff, 0x08000); /* 16k main */
    mmu_init_page (&self->mmu, 0x03, 0xffff, 0x0C000); /* 16k main */
    mmu_init_page (&self->mmu, 0x08, 0xffff, 0x10000); /* 16k video */
    mmu_init_page (&self->mmu, 0x09, 0x0fff, 0x14000); /*  4k video */
    mmu_init_page (&self->mmu, 0x0C, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0D, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0E, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0F, 0x07ff, 0x15000); /*  2k prom */

    mmu_load_page (&self->mmu, 0x00, 0x08); /* video */
    mmu_load_page (&self->mmu, 0x01, 0x09); /* video */
    mmu_load_page (&self->mmu, 0x02, 0x0C); /* prom */
    mmu_load_page (&self->mmu, 0x03, 0x00); /* main */

    /* initialize hardware registers */
    self->control_reg = 0x00;
    self->status1_reg = 0x02; /* no io-board or non-maskable interupts */
    self->status2_reg = 0x80;

    /* initialize keyboard */
    self->cursor_lock = 0;
    self->kb_caps = 0;
    self->kb_mi = 1;
    self->kb_nmi = 1;

    self->kb_count = 0;

    /* initialize floppy disks */
    memcpy (&self->fd[0].data, CPMBASIC_120_NSI, CPMBASIC_120_NSI_len);

    /* initialize PROM */
    self->cpu.pc = 0x8000;
    memcpy (&self->memory[0x15000], ___RESOURCES_ADVPROM_COM, ___RESOURCES_ADVPROM_COM_LEN);

    /* initialize log file */
    s_log = fopen (LOGFILE, "a+");
    if (s_log == NULL)
    {
        fprintf (stderr, "nsae: error, cannot open log file: '%s'\n", LOGFILE);
        return 1;
    }
    fprintf (s_log, "========== NSAE Startup ==========\n");

    return 0;
}

void
adv_quit (void)
{
    fclose (s_log);
    s_log = NULL;
}

int
adv_run (adv_t *self, int number_cycles)
{
    assert (self != NULL);

    return Z80Emulate (&self->cpu, number_cycles, self);
}

uint8_t
adv_read (adv_t *self, uint16_t addr, Z80_STATE *state)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    return self->memory[abs_addr];
}

void
adv_write (adv_t *self, uint16_t addr, uint8_t data, Z80_STATE *state)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    /* dont write to ROM */
    if (abs_addr >= 0x15000) 
    {
        fprintf (s_log, "nsae: debug: failed write to ROM 0x%04X:0x%05X\n", 
                addr, abs_addr);
        return;
    }

    self->memory[abs_addr] = data;
}

uint8_t
adv_in_core (adv_t *self, uint8_t port)
{
    switch (port & 0b11110000)
    {
    case 0x00: /* io board 6 */
    case 0x10: /* io board 5 */
    case 0x20: /* io board 4 */
    case 0x30: /* io board 3 */
    case 0x40: /* io board 2 */
    case 0x50: /* io board 1 */
    case 0x60: /* input main ram parity */
        break;

    case 0xb0: /* clear display flag */
        self->status1_reg &= ~0x04;
        return 0x00;

    case 0xc0: /* clear non-maskable interupt */
        self->status1_reg |= 0x08;
        return 0x00;

    case 0xd0: /* get io status register 2 */
        return self->status2_reg;

    case 0xe0: /* get io status register 1 */
        return self->status1_reg;
    }

    switch (port & 0b11110111)
    {
    case 0x70:  /* id code for slot 6 */
    case 0x71:  /* id code for slot 5 */
    case 0x72:  /* id code for slot 4 */
    case 0x73:  /* id code for slot 3 */
    case 0x74:  /* id code for slot 2 */
    case 0x75:  /* id code for slot 1 */
        return 0xff;

    case 0x76:  /* unsued, returns all 1s */
    case 0x77:
        return 0xff;
    }

    switch (port & 0b11110011)
    {
    case 0x80:  /* get disk data byte */
        return adv_fd_read (self);

    case 0x81:  /* get disk sync byte */
        self->fd_first_read = 1;
        return 0x0fb;

    case 0x82:  /* clear disk read flag */
        self->fd_mode = FD_NONE;
        return 0x00;

    case 0x83:  /* beep */
        printf ("beep! ;TODO: add sound\n");
        return 0x00;
    }

    fprintf (s_log, "nsae: debug: unimplemented in-port: %02x\n", port);
    return 0x00;
}

uint8_t
adv_in (adv_t *self, uint8_t port, Z80_STATE *state)
{
    uint8_t data = adv_in_core (self, port);
    if ((port != 0xd0) && (port != 0xe0))
    {
        printf ("%04x    in  %02x\n", state->pc, port);
        fflush (stdout);
    }
    return data;
}

void
adv_out (adv_t *self, uint8_t port, uint8_t data, Z80_STATE *state)
{
    printf ("%04x    out %02x ;%02x\n", state->pc, port, data);
    fflush (stdout);
    switch (port & 0b11110000)
    {
    case 0x00: /* io board 6 */
    case 0x10: /* io board 5 */
    case 0x20: /* io board 4 */
    case 0x30: /* io board 3 */
    case 0x40: /* io board 2 */
    case 0x50: /* io board 1 */
    case 0x60: /* output main ram parity control bytes */
        break;

    case 0x90: /* load scroll register */
        self->scroll_reg = data & 0x0f;
        return;

    case 0xb0: /* clear display flag */
        self->status1_reg &= ~0x04;
        return;

    case 0xc0: /* clear non-maskable interupt */
        self->status1_reg |= 0x08;
        return;

    case 0xf0: /* output to control register */

        /* io commands */
        switch (data & 0x07)
        {
        case 0x5: /* start drive motors */
        case 0x0: /* show sector */
            self->status2_reg &= 0xf;
            self->status2_reg |= self->fd[self->fd_num].sector;
            self->fd[self->fd_num].sector++;
            self->fd[self->fd_num].sector %= 10;
            break;

        case 0x1: /* show char lsb */
            if (self->kb_count == 0)
            {
                fprintf (s_log, "nsae: warning, reading LSB from empty buffer\n");
            }
            self->status2_reg &= ~0x0f; /* clear loworder bits */
            self->status2_reg |= (self->kb_buf[0] & 0x0f);
            break;

        case 0x2: /* show char msb */
            if (self->kb_count == 0)
            {
                fprintf (s_log, "nsae: warning, reading MSB from empty buffer\n");
            }
            self->status2_reg &= ~0x0f; /* clear loworder bits */
            self->status2_reg |= ((self->kb_buf[0] & 0xf0) >> 4);

            /* shift kb buffer */
            if (self->kb_count > 0)
            {
                self->kb_count--;
                memcpy (self->kb_buf, self->kb_buf+1, self->kb_count);
            }

            /* unset the character data flag */
            if ((self->kb_count == 0) && (self->kb_mi))
            {
                self->status2_reg &= ~0x40;
            }

            /* clear character overrun */
            self->status2_reg &= ~0x20;

            break;

        case 0x3: /* compliment kb mi */
            self->kb_mi ^= 0x01;
            self->status2_reg |= self->kb_mi;
            break;

        case 0x4: /* compliment cursor lock */
            self->cursor_lock ^= 0x01;
            break;

        case 0x6: /* step 1 of compliment kb nmi */
            break;

        case 0x7: 
            /* step 2 of compliment kb nmi */
            if ((self->control_reg & 0x07) == 0x06) 
            {
                self->kb_nmi ^= 0x01;
                self->status2_reg |= self->kb_nmi;
            }
            else /* compliment all caps */
            {
                self->kb_caps ^= 0x01;
            }
            break;
        }

        /* toggle aquire mode */
        if (!(self->control_reg & 0x08) && (data & 0x08))
        {
            /* set the disk serial data bit */
            self->status1_reg |= 0x80;
        }
        else 
        {
            /* unset the bit */
            self->status1_reg &= ~0x80;
        }

        self->control_reg = data;
        self->status2_reg ^= 0x80;
        return;
    }

    switch (port & 0b11110011)
    {
    case 0x80:  /* send a data byte to disk */
        adv_fd_write (self, data);
        return;

    case 0x81:  /* set drive control register */
        adv_fdc_register (self, data);
        return;

    case 0x82:  /* set disk read flag */
        self->fd_mode = FD_READ;
        return;

    case 0x83:  /* set disk write flag */
        self->fd_mode = FD_WRITE;
        return;

    case 0xa0: /* memory mapping reigster */
    case 0xa1: /* memory mapping reigster */
    case 0xa2: /* memory mapping reigster */
    case 0xa3: /* memory mapping reigster */
        adv_set_memory_map (self, port, data);
        return;
    }

    fprintf (s_log, "nsae: debug: unimplemented out-port: %02x\n", port);
}

static void
adv_set_memory_map (adv_t *self, uint8_t port, uint8_t data)
{
    uint8_t page = ((data & 0x80) >> 4) | (data & 0x07);
    uint8_t slot = port & 0x03;

    mmu_load_page (&self->mmu, slot, page);
}

static void
adv_fdc_register (adv_t *self, uint8_t data)
{
    uint8_t fd_num = (data & 0x02);
    floppy_t *p_fd = &self->fd[fd_num];

    /* step floppy drive */
    if (!(self->fdc_reg & 0x10) && (data & 0x10))
    {
        /* inner */
        if (data & 0x20)
        {
            if (p_fd->track < ADV_FD_TRACK_CNT)
            {
                p_fd->track++;
            }
        }
        else /* outer */
        {
            if (p_fd->track != 0)
            {
                p_fd->track--;
            }
        }

        /* set track 0 bit flag */
        if (p_fd->track == 0)
        {
            self->status1_reg |= 0x20; /* set */
        }
        else
        {
            self->status1_reg &= ~0x20; /* unset */
        }
    }

    self->fd_num = fd_num;
    self->fdc_reg = data;
}

static uint8_t *
adv_fd_data_ptr (adv_t *self)
{
    floppy_t *fd = &self->fd[self->fd_num];
    uint32_t offset = 0;

    offset += (self->fdc_reg & 0x40) >> 6;
    offset *= fd->track;
    offset += fd->track;
    offset *= ADV_FD_SECTOR_CNT;
    offset += fd->sector;
    offset *= ADV_FD_BLKSIZE;

    return &fd->data[offset];
}

static uint8_t 
adv_fd_read (adv_t *self)
{
    if (self->fd_mode != FD_READ) return 0x00;
    return *adv_fd_data_ptr (self);
}

static void
adv_fd_write (adv_t *self, uint8_t data)
{
    if (self->fd_mode != FD_WRITE) return;
    *adv_fd_data_ptr (self) = data;
}

/* end of file */
