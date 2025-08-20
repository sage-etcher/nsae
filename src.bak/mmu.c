
#include "mmu.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void
mmu_init_page (mmu_t *self, uint8_t page_num, uint16_t mask, uint32_t base)
{
    assert (self != NULL);
    assert (page_num < MMU_PAGE_COUNT);

    self->masks[page_num] = mask;
    self->bases[page_num] = base;
}

void
mmu_load_page (mmu_t *self, uint8_t slot_num, uint8_t page_num)
{
    assert (self != NULL);
    assert (page_num < MMU_PAGE_COUNT);
    assert (slot_num < MMU_SLOT_COUNT);

    self->slots[slot_num] = page_num;
}

uint32_t
mmu_decode (mmu_t *self, uint16_t addr)
{
    assert (self != NULL);

    /* generate a bitmask to select every bit less than self->slot_bit */
    uint16_t slot_mask = ((uint16_t)1 << self->slot_bit) - 1;

    /* get the page from the address and mask */
    uint8_t slot_num = (addr & ~slot_mask) >> self->slot_bit;
    assert (slot_num < MMU_SLOT_COUNT);

    uint8_t page_num = self->slots[slot_num];
    assert (page_num < MMU_PAGE_COUNT);

    /* calculate the absolute address */
    uint16_t rel_addr = addr & slot_mask;
    uint32_t abs_addr = { 
        ((uint32_t)rel_addr & self->masks[page_num]) + self->bases[page_num],
    };

    return abs_addr;
}

/* end of file */
