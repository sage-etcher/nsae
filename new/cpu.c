
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
    Z80Reset (&self->state);
}

int
cpu_run (cpu_t *self, int cycles, void *cb_data)
{
    return Z80Emulate (&self->state, cycles, cb_data);
}

int
cpu_step (cpu_t *self, void *cb_data)
{
    return cpu_run (self, 1, cb_data);
}



/* end of file */
