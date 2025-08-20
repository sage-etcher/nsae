
#include "crt.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


int
crt_init (crt_t *self, uint8_t *p_vram)
{
    assert (self != NULL);
    assert (p_vram != NULL);

    (void)memset (self, 0, sizeof (*self));

    self->p_vram = p_vram;

    return 0;
}

void
crt_draw (crt_t *self)
{
    assert (self != NULL);
    assert (self->p_vram != NULL);

    const int WIDTH = 240;
    const int HEIGHT = 80;

    /* if blank display, dont update */
    if (self->blank) return;

    /* green */
    glColor3f (0.f, 1.f, 0.f);

    /* for each pixel */
    for (int y = 0; y < WIDTH; y++)
    {
        for (int x = 0; x < HEIGHT; x++)
        {
            uint32_t offset = x * 0x100 + y + self->scroll_reg;
            const unsigned char *p = self->p_vram + offset;
            for (int b = 0; b < 8; b++) /* for each bit (1bit per pixel) */
            {
                /* if pixel is on, draw it */
                if (*p >> b & 1)
                {
                    int screen_x = x * 8 + (7 - b);
                    int screen_y = y;
                    glVertex2f (screen_x,   screen_y);
                    glVertex2f (screen_x+1, screen_y);
                    glVertex2f (screen_x+1, screen_y+1);
                    glVertex2f (screen_x,   screen_y+1);
                }
            }
        }
    }

    /* set display flag */
    self->vrefresh = true;
}

/* end of file */
