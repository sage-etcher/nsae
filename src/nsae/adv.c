
#include "adv.h"

#include "cpu.h"
#include "crt.h"
#include "fdc.h"
#include "io.h"
#include "kb.h"
#include "log.h"
#include "mmu.h"
#include "embed.h"
#include "ram.h"
#include "speaker.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define trap() assert(1==0)

int
adv_init (adv_t *self)
{
    assert (self != NULL);

    int rc = 0;
    rc |= cpu_init (&self->cpu);
    rc |= fdc_init (&self->fdc);
    fdc_load_disk (&self->fdc, 0, "CPMBASIC_120.NSI");

    //rc |= io_init (&self->io);
    rc |= kb_init (&self->kb);
    rc |= speaker_init (&self->speaker);

    /* initialize ram */
    rc |= ram_init (&self->ram, EMBED_PROM, EMBED_PROM_LEN);

    /* initialize mmu */
    rc |= mmu_init (&self->mmu, 14, &self->ram);

    mmu_init_page (&self->mmu, 0x00, 0xffff, RAM_BASE_MAIN_0);
    mmu_init_page (&self->mmu, 0x01, 0xffff, RAM_BASE_MAIN_1);
    mmu_init_page (&self->mmu, 0x02, 0xffff, RAM_BASE_MAIN_2);
    mmu_init_page (&self->mmu, 0x03, 0xffff, RAM_BASE_MAIN_3);
    mmu_init_page (&self->mmu, 0x08, 0xffff, RAM_BASE_VRAM_8);
    mmu_init_page (&self->mmu, 0x09, 0x3fff, RAM_BASE_VRAM_9);
    mmu_init_page (&self->mmu, 0x0C, 0x07ff, RAM_BASE_PROM);
    mmu_init_page (&self->mmu, 0x0D, 0x07ff, RAM_BASE_PROM);
    mmu_init_page (&self->mmu, 0x0E, 0x07ff, RAM_BASE_PROM);
    mmu_init_page (&self->mmu, 0x0F, 0x07ff, RAM_BASE_PROM);

    mmu_load_page (&self->mmu, 0x00, 0x08);
    mmu_load_page (&self->mmu, 0x01, 0x09);
    mmu_load_page (&self->mmu, 0x02, 0x0C);
    mmu_load_page (&self->mmu, 0x03, 0x00);

    /* inialize the display */
    rc |= crt_init (&self->crt, &self->ram, RAM_BASE_VRAM_8);

    if (rc != 0)
    {
        log_error ("nsae: adv: failed to initialize\n");
        return 1;
    }

    self->cpu.state.pc = 0x8000;

    self->kb_nmi = true;
    self->kb_mi  = true;
    self->crt_mi = false;

    self->hw_interupt = true;

    self->cmd_ack   = false;
    self->ctrl_reg  = 0;
    self->stat1_reg = 0;
    self->stat2_reg = 0;

    return 0;
}

int
adv_run (adv_t *self, int cycles)
{
    assert (self != NULL);

    return cpu_run (&self->cpu, cycles, self);
}

int
adv_step (adv_t *self)
{
    assert (self != NULL);

    return cpu_step (&self->cpu, self);
}

static void
adv_update_status (adv_t *self)
{
    uint8_t *stat1 = &self->stat1_reg;
    uint8_t *stat2 = &self->stat2_reg;
    crt_t *crt = &self->crt;
    fdc_t *fdc = &self->fdc;
    kb_t *kb = &self->kb;

    /* status register 1 */
    *stat1 &= 0x01;

    /* bit7 disk serial data flag */
    /* bit6 sector mark */
    /* bit5 track 0 */
    *stat1 |= fdc->track_zero << 5;

    /* bit4 disk write protect */
    *stat1 |= fdc->hard_ro << 4;

    /* bit3 non-maskable interupt */
    *stat1 |= (!self->hw_interupt & 0x01) << 3;

    /* bit2 display flag */
    *stat1 |= (self->crt_mi & crt->vrefresh) << 2;

    /* bit1 io interrupt */
    /* bit0 command output (externally managed) */

    /* status register 2 */
    *stat2 &= 0x0f;

    /* bit 7 command acknowledge */
    *stat2 |= self->cmd_ack << 7;

    /* bit 6 keyboard data flag */
    *stat2 |= (self->kb_mi & kb->data_flag) << 6;

    /* bit 5 character overrun */
    *stat2 |= (self->kb_mi & kb->overflow) << 5;

    /* bit 4 auto repeat */
    *stat2 |= (self->kb_mi & kb->autorepeat) << 4;

    /* bit 3-0 command outputs (externally managed) */

    //log_debug ("status1_reg = %d%d%d%d$%d%d%d%d\n",
    //        (*stat1 >> 7) & 0x1, (*stat1 >> 6) & 0x1,
    //        (*stat1 >> 5) & 0x1, (*stat1 >> 4) & 0x1,
    //        (*stat1 >> 3) & 0x1, (*stat1 >> 2) & 0x1,
    //        (*stat1 >> 1) & 0x1, (*stat1 >> 0) & 0x1);

    //log_debug ("status2_reg = %d%d%d%d$%d%d%d%d\n",
    //        (*stat2 >> 7) & 0x0, (*stat2 >> 6) & 0x1,
    //        (*stat2 >> 5) & 0x1, (*stat2 >> 4) & 0x1,
    //        (*stat2 >> 3) & 0x1, (*stat2 >> 2) & 0x1,
    //        (*stat2 >> 1) & 0x1, (*stat2 >> 0) & 0x1);
}

uint8_t
adv_in (adv_t *self, uint8_t port, uint16_t pc)
{
    adv_update_status (self);
    //log_debug ("%04x    in %02x\n", pc, port);

    switch (port & 0xf0)
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
        self->crt.vrefresh = 0;
        return 0x00;

    case 0xc0: /* clear non-maskable interupt */
        self->hw_interupt = 0;
        return 0x00;

    case 0xd0: /* get io stat register 2 */
        return self->stat2_reg;

    case 0xe0: /* get io stat register 1 */
        return self->stat1_reg;
    }

    switch (port & 0xf7)
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

    switch (port & 0xf3)
    {
    case 0x80:  /* get disk data byte */
        return fdc_read (&self->fdc);

    case 0x81:  /* get disk sync byte */
        return fdc_read_sync1 (&self->fdc);

    case 0x82:  /* clear disk read flag */
        self->fdc.read_mode = false;
        return 0x00;

    case 0x83:  /* beep */
        speaker_beep (&self->speaker);
        return 0x00;
    }

    log_warning ("nsae: debug: unimplemented in-port: %02x\n", port);
    return 0x00;
}

static void 
adv_command (adv_t *self, uint8_t data)
{
    /* io commands */
    self->stat2_reg &= ~0x0f; /* clear loworder bits */

    switch (data & 0x07)
    {
    case 0x5: /* start drive motors */
        fdc_start_motor (&self->fdc);
        /* fall through */
    case 0x0: /* show sector */
        self->stat2_reg &= 0x0f;
        self->stat2_reg |= fdc_get_sector (&self->fdc) & 0x0f;
        break;

    case 0x1: /* show char lsb */
        self->stat2_reg |= kb_get_lsb (&self->kb);
        break;

    case 0x2: /* show char msb */
        self->stat2_reg |= kb_get_msb (&self->kb);
        break;

    case 0x3: /* compliment kb mi */
        self->kb_mi ^= 0x01;

        self->stat1_reg &= ~0x01;
        self->stat1_reg |= self->kb_mi;
        break;

    case 0x4: /* compliment cursor lock */
        self->kb.cursor_lock ^= 0x01;

        self->stat2_reg &= ~0x01;
        self->stat2_reg |= self->kb.cursor_lock;
        break;

    case 0x6: /* step 1 of compliment kb nmi */
        break;

    case 0x7: 
        /* step 2 of compliment kb nmi */
        if ((self->ctrl_reg & 0x07) == 0x06) 
        {
            self->kb_nmi ^= 0x01;

            self->stat2_reg &= ~0x01;
            self->stat2_reg |= self->kb_nmi;
        }
        else /* compliment all caps */
        {
            self->kb.caps_lock ^= 0x01;

            self->stat2_reg &= ~0x01;
            self->stat2_reg |= self->kb.caps_lock;
        }
        break;
    }

    ///* toggle aquire mode */
    //if (!(self->ctrl_reg & 0x08) && (data & 0x08))
    //{
    //    /* set the disk serial data bit */
    //    self->stat1_reg |= 0x80;
    //}
    //else 
    //{
    //    /* unset the bit */
    //    self->stat1_reg &= ~0x80;
    //}

    self->ctrl_reg = data;
    self->cmd_ack ^= 0x01;
    //self->stat2_reg ^= 0x80;
    //log_debug ("status1_reg = %d%d%d%d$%d%d%d%d\n",
    //        (self->stat1_reg >> 7) & 0x1, (self->stat1_reg >> 6) & 0x1,
    //        (self->stat1_reg >> 5) & 0x1, (self->stat1_reg >> 4) & 0x1,
    //        (self->stat1_reg >> 3) & 0x1, (self->stat1_reg >> 2) & 0x1,
    //        (self->stat1_reg >> 1) & 0x1, (self->stat1_reg >> 0) & 0x1);

    //log_debug ("status2_reg = %d%d%d%d$%d%d%d%d\n",
    //        (self->stat2_reg >> 7) & 0x0, (self->stat2_reg >> 6) & 0x1,
    //        (self->stat2_reg >> 5) & 0x1, (self->stat2_reg >> 4) & 0x1,
    //        (self->stat2_reg >> 3) & 0x1, (self->stat2_reg >> 2) & 0x1,
    //        (self->stat2_reg >> 1) & 0x1, (self->stat2_reg >> 0) & 0x1);
}


void
adv_out (adv_t *self, uint8_t port, uint8_t data, uint16_t pc)
{
    uint8_t a = 0;
    uint8_t b = 0;

    adv_update_status (self);
    //log_debug ("%04x    out %02x ;%d%d%d%d$%d%d%d%d\n",
    //        pc, port,
    //        (data >> 7) & 0x01, (data >> 6) & 0x01,
    //        (data >> 5) & 0x01, (data >> 4) & 0x01,
    //        (data >> 3) & 0x01, (data >> 2) & 0x01,
    //        (data >> 1) & 0x01, (data >> 0) & 0x01);

    switch (port & 0xf0)
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
        self->crt.scroll_reg = data & 0x0f;
        return;

    case 0xb0: /* clear display flag */
        self->crt.vrefresh = 0;
        return;

    case 0xc0: /* clear non-maskable interupt */
        self->hw_interupt = 0;
        return;

    case 0xf0: /* output to control register */
        /* bit 7 display interupt enable */
        self->crt_mi = (data >> 7) & 0x01;

        /* bit 6 speaker data */
        /* bit 5 blank display */
        self->crt.blank = (data >> 5) & 0x01;

        /* bit 4 io reset */
        if ((~data >> 4) & 0x01)
        {
            /* kb */
            self->stat1_reg |= 0x01;
            self->kb_mi = 1;
            self->kb.buf_cnt = 0;
            self->kb.data_flag = 0;
            self->kb.cursor_lock = 0;
            self->kb.caps_lock = 0;
            self->kb.autorepeat = 0;
            self->kb.overflow = 0;

            /* disk */ 
            /* io boards */
        }

        /* bit 3 aquire mode */


        adv_command (self, data);
        return;
    }

    switch (port & 0xf3)
    {
    case 0x80:  /* send a data byte to disk */
        fdc_write (&self->fdc, data);
        return;

    case 0x81:  /* set drive control register */
        /* bit 0-1 disk select */
        self->fdc.disk = (data >> 1) & 0x01;

        /* bit 5 precompensation and step_direction */
        self->fdc.step_direction = (data >> 5) & 0x01;
        self->fdc.precompensation = self->fdc.step_direction;

        /* bit 6 side select */
        self->fdc.side = (data >> 6) & 0x01;

        /* bit 4 step pulse */
        if (!self->fdc.step_pulse && ((data >> 4) & 0x01))
        {
            fdc_step (&self->fdc);
        }
        self->fdc.step_pulse = (data >> 4) & 0x01;

        return;

    case 0x82:  /* set disk read flag */
        self->fdc.read_mode = true;
        return;

    case 0x83:  /* set disk write flag */
        self->fdc.write_mode = true;
        return;

    case 0xa0: /* memory mapping reigster */
    case 0xa1: /* memory mapping reigster */
    case 0xa2: /* memory mapping reigster */
    case 0xa3: /* memory mapping reigster */
        a = port & 0x03;
        b = ((data & 0x80) >> 4) | (data & 0x07);
        mmu_load_page (&self->mmu, a, b);
        return;
    }

    log_warning ("nsae: debug: unimplemented out-port: %02x\n", port);
}


/* end of file */
