
#define LOG_CATEGORY LC_CRT
#include "crt.h"

#include "nslog.h"
#include "ram.h"

#include <GL/glew.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define TX_WIDTH  640
#define TX_HEIGHT 256

#define WIDTH  640
#define HEIGHT 240

int
crt_init (crt_t *self, ram_t *p_ram, uint32_t vram_offset)
{
    assert (self != NULL);
    assert (p_ram != NULL);

    (void)memset (self, 0, sizeof (*self));

    self->foreground[0] = 0.38f; 
    self->foreground[1] = 1.0f; 
    self->foreground[2] = 0.0f;

    self->background[0] = 0.0f;  
    self->background[1] = 0.0f; 
    self->background[2] = 0.0f;

    glGenTextures(1, &self->h_texture);
    glBindTexture(GL_TEXTURE_2D, self->h_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TX_WIDTH, TX_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return 0;
}

void
crt_draw (crt_t *self)
{
    assert (self != NULL);
    assert (self->p_ram!= NULL);

    glClearColor (
            self->background[0], 
            self->background[1], 
            self->background[2], 
            1.f);
    glBindTexture(GL_TEXTURE_2D, self->h_texture);

    glClear (GL_COLOR_BUFFER_BIT);

    /* if blank display, dont update */
    if (self->blank) return;

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    const uint16_t WRAP_OFFSET = TX_HEIGHT - HEIGHT;

    vec2_t top_clip, btm_clip;
    vec2_t top_scrn, btm_scrn;

    // upper half
    top_clip[0] = (GLfloat)self->scroll_reg;
    top_clip[1] = (GLfloat)TX_HEIGHT;

    top_scrn[0] = (GLfloat)0;
    top_scrn[1] = (GLfloat)HEIGHT-(self->scroll_reg-WRAP_OFFSET);

    glTexCoord2f(0, top_clip[0]/TX_HEIGHT); glVertex2f(0,     top_scrn[0]);
    glTexCoord2f(0, top_clip[1]/TX_HEIGHT); glVertex2f(0,     top_scrn[1]);
    glTexCoord2f(1, top_clip[1]/TX_HEIGHT); glVertex2f(WIDTH, top_scrn[1]);
    glTexCoord2f(1, top_clip[0]/TX_HEIGHT); glVertex2f(WIDTH, top_scrn[0]);

    // lower half
    btm_clip[0] = (GLfloat)0;
    btm_clip[1] = (GLfloat)self->scroll_reg-WRAP_OFFSET;

    btm_scrn[0] = (GLfloat)HEIGHT-(self->scroll_reg-WRAP_OFFSET);
    btm_scrn[1] = (GLfloat)HEIGHT;

    glTexCoord2f(0, btm_clip[0]/TX_HEIGHT); glVertex2f(0,     btm_scrn[0]);
    glTexCoord2f(0, btm_clip[1]/TX_HEIGHT); glVertex2f(0,     btm_scrn[1]);
    glTexCoord2f(1, btm_clip[1]/TX_HEIGHT); glVertex2f(WIDTH, btm_scrn[1]);
    glTexCoord2f(1, btm_clip[0]/TX_HEIGHT); glVertex2f(WIDTH, btm_scrn[0]);

    glDisable(GL_TEXTURE_2D);
    glEnd();

    /* set display flag */
    self->vrefresh = true;
}

void
crt_update (crt_t *self, uint32_t ram_addr, uint8_t data)
{
    if (ram_addr < RAM_BASE_VRAM_8 || ram_addr >= RAM_BASE_PROM)
        return;
    
    log_debug("crt_update %p %x %x\n", self, ram_addr, data);
    
    uint16_t offset = ram_addr - RAM_BASE_VRAM_8;
    uint8_t x = offset / 0x100;
    uint8_t y = offset % 0x100;
        
    int screen_x = x * 8;
    int screen_y = y;

    GLfloat update[8*3];

    for (int b = 0; b < 8; b++)
    {
        GLfloat *pixel  = (data >> b & 1) ? self->foreground : self->background;
        memcpy (update + 3*(7-b), pixel, 3 * sizeof(GLfloat));
    }

    glBindTexture(GL_TEXTURE_2D, self->h_texture);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        screen_x, screen_y,
        8, 1,
        GL_RGB, GL_FLOAT, update);
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
