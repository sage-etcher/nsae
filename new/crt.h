
#ifndef NSAE_VIDEO_H
#define NSAE_VIDEO_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool blank;
    bool vrefresh;
    uint8_t scroll_reg;
    const uint8_t *p_vram;
} crt_t;

int crt_init (crt_t *self, uint8_t *p_vram);

void crt_draw (crt_t *self);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_VIDEO_H */
/* end of file */
