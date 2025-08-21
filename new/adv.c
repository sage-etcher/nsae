
#include "adv.h"

#include "cpu.h"
#include "crt.h"
#include "fdc.h"
#include "io.h"
#include "kb.h"
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
    //rc |= fdc_init (&self->fdc);
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
        fprintf (stderr, "nsae: adv: failed to initialize\n");
        return 1;
    }

    self->hw_interupt = false;

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

uint8_t
adv_in (adv_t *self, uint8_t port)
{
    trap();
    return 0x00;
}

void
adv_out (adv_t *self, uint8_t port, uint8_t data)
{
    trap();
    return;
}


/* end of file */
