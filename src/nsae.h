
#ifndef NSAE_H
#define NSAE_H

#include "mmu.h"
#include "z80emu.h"

#include <stdint.h>

#define ADV_RAM (64+20+2) * (2<<10)
typedef struct {
    Z80_STATE cpu;
    mmu_t mmu;
    uint8_t memory[ADV_RAM];
    uint8_t scroll_reg;
} adv_t;

int adv_init (adv_t *self);

int adv_run (adv_t *self, int number_cycles);

uint8_t adv_read  (adv_t *self, uint16_t addr);
void    adv_write (adv_t *self, uint16_t addr, uint8_t data);

uint8_t adv_in  (adv_t *self, uint8_t port);
void    adv_out (adv_t *self, uint8_t port, uint8_t data);

#endif
/* end of file */
