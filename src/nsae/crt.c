
#define LOG_CATEGORY LC_CRT
#include "crt.h"

#include "nslog.h"
#include "ram.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


int
crt_init (crt_t *self, ram_t *p_ram, uint32_t vram_offset)
{
    assert (self != NULL);
    assert (p_ram != NULL);

    (void)memset (self, 0, sizeof (*self));

    self->p_ram = p_ram;
    self->vram_offset = vram_offset;

    return 0;
}

void
crt_draw (crt_t *self)
{
    assert (self != NULL);
    assert (self->p_ram!= NULL);

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
            uint32_t offset = (x * 0x100) + ((y + self->scroll_reg) & 0xff);
            uint32_t abs_addr = self->vram_offset + offset;
            uint8_t data = ram_read (self->p_ram, abs_addr);

            for (int b = 0; b < 8; b++) /* for each bit (1bit per pixel) */
            {
                /* if pixel is on, draw it */
                if (data >> b & 1)
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

void
crt_status (crt_t *self)
{
    log_info ("blank: %1d\tvrefresh: %1d\tscroll_reg: %d\n",
            self->blank,
            self->vrefresh,
            self->scroll_reg);
}

/* end of file */
