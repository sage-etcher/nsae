
#ifndef NSAE_MMU_H
#define NSAE_MMU_H
#ifdef __cplusplus
extern "c" {
#endif

#include "ram.h"

#include <stdbool.h>
#include <stdint.h>

#define MMU_SLOT_CNT 4
#define MMU_PAGE_CNT 16
typedef struct {
    ram_t *p_ram;
    uint8_t slots[MMU_SLOT_CNT];
    uint32_t bases[MMU_PAGE_CNT];
    uint16_t masks[MMU_PAGE_CNT];
} mmu_t;

int mmu_init (mmu_t *self, ram_t *p_ram);
void mmu_init_page (mmu_t *self, uint8_t page, uint32_t base, uint16_t mask);
void mmu_load_page (mmu_t *self, uint8_t slot, uint8_t page);

uint8_t *mmu_deref (mmu_t *self, uint16_t addr);
uint8_t read (mmu_t *self, uint16_t addr);
void write (mmu_t *self, uint16_t addr, uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_MMU_H */
/* end of file */
