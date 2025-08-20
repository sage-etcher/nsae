
#include "mmu.h"

#include "ram.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static uint32_t mmu_decode (mmu_t *self, uint16_t addr);


int
mmu_init (mmu_t *self, uint8_t slot_bit, ram_t *p_ram)
{
    assert (self != NULL);
    assert (p_ram != NULL);
    assert (slot_bit > 0 && slot_bit < 16);

    (void)memset (self, 0, sizeof (*self));

    self->slot_bit = slot_bit;
    self->p_ram = p_ram;

    return 0;
}

void
mmu_init_page (mmu_t *self, uint8_t page, uint16_t mask, uint32_t base)
{
    assert (self != NULL);
    assert (page < MMU_PAGE_CNT);

    self->bases[page] = base;
    self->masks[page] = mask;
}

void
mmu_load_page (mmu_t *self, uint8_t slot, uint8_t page)
{
    assert (self != NULL);
    assert (slot < MMU_SLOT_CNT);
    assert (page < MMU_PAGE_CNT);

    self->slots[slot] = page;
}

static uint32_t
mmu_decode (mmu_t *self, uint16_t addr)
{
    assert (self != NULL);

    /* generate a bitmask to select every bit less than self->slot_bit */
    uint16_t slot_mask = ((uint16_t)1 << self->slot_bit) - 1;

    /* get the page from the address and mask */
    uint8_t slot_num = (addr & ~slot_mask) >> self->slot_bit;
    assert (slot_num < MMU_SLOT_CNT);

    uint8_t page_num = self->slots[slot_num];
    assert (page_num < MMU_PAGE_CNT);

    /* calculate the absolute address */
    uint16_t rel_addr = addr & slot_mask;
    uint32_t abs_addr = { 
        ((uint32_t)rel_addr & self->masks[page_num]) + self->bases[page_num],
    };

    return abs_addr;
}

uint8_t
mmu_read (mmu_t *self, uint16_t addr)
{
    assert (self != NULL);
    assert (self->p_ram != NULL);

    uint32_t abs_addr = mmu_decode (self, addr);

    return ram_read (self->p_ram, abs_addr);
}

void
mmu_write (mmu_t *self, uint16_t addr, uint8_t data)
{
    assert (self != NULL);
    assert (self->p_ram != NULL);

    uint32_t abs_addr = mmu_decode (self, addr);

    ram_write (self->p_ram, abs_addr, data);
}


/* end of file */
