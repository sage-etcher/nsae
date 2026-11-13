
#ifndef NSAE_VIDEO_H
#define NSAE_VIDEO_H
#ifdef __cplusplus
extern "c" {
#endif

#include "glinit.h"
#include "ram.h"

#include <stdbool.h>
#include <stdint.h>

typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];
typedef GLfloat vec4_t[4];

typedef struct {
    bool blank;
    bool vrefresh;
    uint8_t scroll_reg;
    uint32_t vram_offset;
    ram_t *p_ram;

    vec3_t background;
    vec3_t foreground;

    GLuint h_texture;
} crt_t;

int crt_init (crt_t *self, ram_t *p_ram, uint32_t vram_offset);

void crt_draw (crt_t *self);

void crt_update (crt_t *self, uint32_t ram_addr, uint8_t data);

void crt_status (crt_t *self);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_VIDEO_H */
/* end of file */
