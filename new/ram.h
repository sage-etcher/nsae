
#ifndef NSAE_MEMORY_H
#define NSAE_MEMORY_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define RAM_BASE_MAIN_0 0x00000
#define RAM_BASE_MAIN_1 0x04000
#define RAM_BASE_MAIN_2 0x08000
#define RAM_BASE_MAIN_3 0x0C000
#define RAM_BASE_VRAM_8 0x10000
#define RAM_BASE_VRAM_9 0x14000
#define RAM_BASE_PROM_C 0x15000
#define RAM_SIZE        0x15800

typedef struct {
    uint8_t m[RAM_SIZE];
} ram_t;

int ram_init (ram_t *self);

uint8_t ram_read (ram_t *self, uint32_t addr);
void ram_write (ram_t *self, uint32_t addr, uint8_t data);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_MEMORY_H */
/* end of file */
