
#include "adv.h"

#include "cpu.h"
#include "crt.h"
#include "fdc.h"
#include "io.h"
#include "kb.h"
#include "mmu.h"
#include "ram.h"
#include "speaker.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


int
adv_init (adv_t *self)
{
    assert (self != NULL);

    int rc = 0;
    rc |= cpu_init (&self->cpu);
    rc |= crt_init (&self->crt);
    rc |= fdc_init (&self->fdc);
    rc |= io_init (&self->io);
    rc |= kb_init (&self->kb);
    rc |= mmu_init (&self->mmu);
    rc |= ram_init (&self->ram);
    rc |= speaker_init (&self->speaker);

    if (rc != 0)
    {
        return 1;
    }

    self->hw_interupt = false;

    self->cmd_ack   = false;
    self->ctrl_reg  = 0;
    self->stat1_reg = 0;
    self->stat2_reg = 0;

    return 0;
}

uint8_t
adv_in (uint8_t port)
{
    return 0x00;
}

void
adv_out (uint8_t port, uint8_t data)
{
    return;
}


/* end of file */
