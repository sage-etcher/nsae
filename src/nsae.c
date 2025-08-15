
#include "nsae.h"

#include "advprom.h"
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


int
adv_init (adv_t *self)
{
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
adv_read (adv_t *self, uint16_t addr)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    return self->memory[abs_addr];
}

void
adv_write (adv_t *self, uint16_t addr, uint8_t data)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    /* dont write to ROM */
    if (abs_addr >= 0x15000) 
    {
        fprintf (s_log, "nsae: debug: failed write to ROM 0x%05X\n", abs_addr);
        return;
    }

    self->memory[abs_addr] = data;
}

uint8_t
adv_in (adv_t *self, uint8_t port)
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
    case 0x81:  /* get disk sync byte */
    case 0x82:  /* clear disk read flag */
        break;

    case 0x83:  /* beep */
        printf ("beep! ;TODO: add sound\n");
        return 0x00;
    }

    fprintf (s_log, "nsae: debug: unimplemented in-port: %02x\n", port);
    return 0x00;
}

void
adv_out (adv_t *self, uint8_t port, uint8_t data)
{
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
        self->control_reg = data;
        return;
    }

    switch (port & 0b11110011)
    {
    case 0x80:  /* send a data byte to disk */
    case 0x81:  /* set drive control register */
    case 0x82:  /* set disk read flag */
    case 0x83:  /* set disk write flag */
        break;

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


/* end of file */
