
#ifndef NSAE_VIDEO_H
#define NSAE_VIDEO_H
#ifdef __cplusplus
extern "c" {
#endif

#include "glinit.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct { GLfloat r, g, b; } color_t;

typedef struct { GLfloat x, y; } vec2_t;
typedef struct { GLfloat x, y, z; } vec3_t;
typedef struct { GLfloat x, y, z, w; } vec4_t;

typedef struct { GLint x, y; } ivec2_t;

#define TX_WIDTH  640
#define TX_HEIGHT 256
#define TX_SIZE   TX_WIDTH * TX_HEIGHT * sizeof (GLfloat) * 3

#define WIDTH  640
#define HEIGHT 240

typedef struct {
    uint8_t scroll_reg;
    bool blank;
    bool vrefresh;
    bool tex_sync;

    color_t background;
    color_t foreground;

    GLfloat *texture;
    GLuint h_texture;

    ivec2_t sub_start;
    ivec2_t sub_end;
} crt_t;

int crt_init (crt_t *self);

void crt_free (crt_t *self);

void crt_draw (crt_t *self);

void crt_update (crt_t *self, uint32_t ram_addr, uint8_t data);

void crt_status (crt_t *self);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_VIDEO_H */
/* end of file */
