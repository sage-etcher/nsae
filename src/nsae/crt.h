
#ifndef NSAE_VIDEO_H
#define NSAE_VIDEO_H
#ifdef __cplusplus
extern "c" {
#endif

#include "ram.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool blank;
    bool vrefresh;
    uint8_t scroll_reg;
    uint32_t vram_offset;
    ram_t *p_ram;
} crt_t;

int crt_init (crt_t *self, ram_t *p_ram, uint32_t vram_offset);

void crt_draw (crt_t *self);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_VIDEO_H */
/* end of file */
