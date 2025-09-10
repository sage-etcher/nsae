
#define LOG_CATEGORY LC_CPU
#include "cpu.h"

#include "mmu.h"
#include "nslog.h"
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
    log_debug ("nsae: cpu: reset\n");
    Z80Reset (&self->state);
}

int
cpu_run (cpu_t *self, int cycles, void *cb_data)
{
    log_debug ("nsae: cpu: run %d\n", cycles);
    return Z80Emulate (&self->state, cycles, cb_data);
}

int
cpu_step (cpu_t *self, void *cb_data)
{
    log_debug ("nsae: cpu: step\n");
    return cpu_run (self, 1, cb_data);
}

void
cpu_status (cpu_t *self, mmu_t *mmu)
{
    log_info ("status: %d\talternates: %04x %04x %04x %04x\n",
            self->state.status,
            self->state.alternates[0],
            self->state.alternates[1],
            self->state.alternates[2],
            self->state.alternates[3]
    );
    log_info ("AF: %02x %02x\tBC: %02x %02x\tDE: %02x %02x\tHL: %02x %02x\n",
            self->state.registers.byte[Z80_A],
            self->state.registers.byte[Z80_F],
            self->state.registers.byte[Z80_B],
            self->state.registers.byte[Z80_C],
            self->state.registers.byte[Z80_D],
            self->state.registers.byte[Z80_E],
            self->state.registers.byte[Z80_H],
            self->state.registers.byte[Z80_L]
    );
    log_info ("IX: %04x\tIY: %04x\tSP: %04x *%02x%02x\tPC: %04x *%02x\n",
            self->state.registers.word[Z80_IX],
            self->state.registers.word[Z80_IY],
            self->state.registers.word[Z80_SP],
            mmu_read (mmu, self->state.registers.word[Z80_SP]+1),
            mmu_read (mmu, self->state.registers.word[Z80_SP]),
            self->state.pc,
            mmu_read (mmu, self->state.pc)
    );
    log_info ("I: %04x  R: %04x  IFF1: %04x  IFF2: %04x  IM: %04x\n",
            self->state.i,
            self->state.r,
            self->state.iff1,
            self->state.iff2,
            self->state.im
    );
}


/* end of file */
