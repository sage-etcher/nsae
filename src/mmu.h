
#ifndef MMU_H
#define MMU_H

#define MMU_PAGE_COUNT 16
#define MMU_SLOT_COUNT 4

#include <stdint.h>

typedef struct {
    uint16_t masks[MMU_PAGE_COUNT];
    uint32_t bases[MMU_PAGE_COUNT];
    uint8_t  slot_bit;
    uint8_t  slots[MMU_SLOT_COUNT];
} mmu_t;


void mmu_init_page (mmu_t *self, uint8_t page_num, uint16_t mask,
                    uint32_t base);

void mmu_load_page (mmu_t *self, uint8_t slot_num, uint8_t page_num);

uint32_t mmu_decode (mmu_t *self, uint16_t addr);


#endif
/* end of file */
