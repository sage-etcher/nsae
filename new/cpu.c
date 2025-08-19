
#include "cpu.h"

#include "z80emu.h"

#include <stdbool.h>
#include <stdint.h>

int
cpu_init (cpu_t *self)
{
    cpu_reset (self);

    return 0;
}

void
cpu_reset (cpu_t *self)
{
    Z80Reset (&self->state)
}

uint32_t
cpu_step (cpu_t *self)
{
    return 0;
}

uint32_t
cpu_run (cpu_t *self, uint32_t cycles)
{
    return 0;
}


/* end of file */
