
#define LOG_CATEGORY LC_MMU
#include "mmu.h"

#include "nslog.h"
#include "ram.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


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

    log_debug ("nsae: mmu: mapped page %1x into slot %1x\n", page, slot);
}

uint32_t
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

    log_debug ("nsae: mmu: decode %04x (%05x)\n", addr, abs_addr);

    return abs_addr;
}

uint8_t
mmu_read (mmu_t *self, uint16_t addr)
{
    assert (self != NULL);
    assert (self->p_ram != NULL);

    uint32_t abs_addr = mmu_decode (self, addr);

    log_debug ("nsae: mmu: read %04x (%05x)\n", addr, abs_addr);

    return ram_read (self->p_ram, abs_addr);
}

void
mmu_write (mmu_t *self, uint16_t addr, uint8_t data)
{
    assert (self != NULL);
    assert (self->p_ram != NULL);

    uint32_t abs_addr = mmu_decode (self, addr);

    log_debug ("nsae: mmu: write %04x (%05x) %02x\n", addr, abs_addr, data);

    ram_write (self->p_ram, abs_addr, data);
}

void
mmu_status (mmu_t *self)
{
    log_info ("0: %1x %05x 0x0000\n", 
            self->slots[0],
            self->bases[self->slots[0]]);
    log_info ("1: %1x %05x 0x4000\n", 
            self->slots[1],
            self->bases[self->slots[1]]);
    log_info ("2: %1x %05x 0x8000\n", 
            self->slots[2],
            self->bases[self->slots[2]]);
    log_info ("3: %1x %05x 0xC000\n", 
            self->slots[3],
            self->bases[self->slots[3]]);
}


/* end of file */
