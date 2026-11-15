
#define LOG_CATEGORY LC_CRT
#include "crt.h"

#include "nslog.h"
#include "ram.h"

#include <GL/glew.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

static inline GLfloat *texture_idx(crt_t *self, GLint x, GLint y);
static void crt_update_texture(crt_t *self);

int
crt_init (crt_t *self)
{
    assert (self != NULL);

    (void)memset (self, 0, sizeof (*self));

    self->foreground = (color_t){ .r = 0.38f, .g = 1.00f, .b = 0.00f }; 
    self->background = (color_t){ .r = 0.00f, .g = 0.00f, .b = 0.00f }; 

    self->texture = malloc (TX_SIZE);

    glGenTextures(1, &self->h_texture);
    glBindTexture(GL_TEXTURE_2D, self->h_texture);
    glTexImage2D(
            GL_TEXTURE_2D, 0, 
            GL_RGB, 
            TX_WIDTH, TX_HEIGHT, 
            0, 
            GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return 0;
}

void
crt_free (crt_t *self)
{
    free (self->texture);
    self->texture = NULL;
}

void
crt_draw (crt_t *self)
{
    assert (self != NULL);

    crt_update_texture (self);

    glClearColor (
            self->background.r, 
            self->background.g, 
            self->background.b, 
            1.f);
    glBindTexture (GL_TEXTURE_2D, self->h_texture);

    glClear (GL_COLOR_BUFFER_BIT);

    /* if blank display, dont update */
    if (self->blank) return;

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    const uint16_t WRAP_OFFSET = TX_HEIGHT - HEIGHT;

    vec2_t top_clip, btm_clip;
    vec2_t top_scrn, btm_scrn;

    // upper half
    top_clip.x = (GLfloat)self->scroll_reg;
    top_clip.y = (GLfloat)TX_HEIGHT;

    top_scrn.x = (GLfloat)0;
    top_scrn.y = (GLfloat)HEIGHT-(self->scroll_reg-WRAP_OFFSET);

    glTexCoord2f(0, top_clip.x/TX_HEIGHT); glVertex2f(0,     top_scrn.x);
    glTexCoord2f(0, top_clip.y/TX_HEIGHT); glVertex2f(0,     top_scrn.y);
    glTexCoord2f(1, top_clip.y/TX_HEIGHT); glVertex2f(WIDTH, top_scrn.y);
    glTexCoord2f(1, top_clip.x/TX_HEIGHT); glVertex2f(WIDTH, top_scrn.x);

    // lower half
    btm_clip.x = (GLfloat)0;
    btm_clip.y = (GLfloat)self->scroll_reg-WRAP_OFFSET;

    btm_scrn.x = (GLfloat)HEIGHT-(self->scroll_reg-WRAP_OFFSET);
    btm_scrn.y = (GLfloat)HEIGHT;

    glTexCoord2f(0, btm_clip.x/TX_HEIGHT); glVertex2f(0,     btm_scrn.x);
    glTexCoord2f(0, btm_clip.y/TX_HEIGHT); glVertex2f(0,     btm_scrn.y);
    glTexCoord2f(1, btm_clip.y/TX_HEIGHT); glVertex2f(WIDTH, btm_scrn.y);
    glTexCoord2f(1, btm_clip.x/TX_HEIGHT); glVertex2f(WIDTH, btm_scrn.x);

    glDisable(GL_TEXTURE_2D);
    glEnd();

    /* set display flag */
    self->vrefresh = true;
}


static inline GLfloat *
texture_idx(crt_t *self, GLint x, GLint y)
{
    return self->texture + ((y * WIDTH) + x) * 3;
}

void
crt_update (crt_t *self, uint32_t ram_addr, uint8_t data)
{
    if (ram_addr < RAM_BASE_VRAM_8 || ram_addr >= RAM_BASE_PROM)
        return;
    
    log_debug("crt_update %p %x %x\n", self, ram_addr, data);
    
    uint16_t offset = ram_addr - RAM_BASE_VRAM_8;
    GLint x = (GLint)offset / 0x100 * 8;
    GLint y = (GLint)offset % 0x100;

    for (int b = 0; b < 8; b++)
    {
        color_t *pixel  = (data >> b & 1) 
                            ? &self->foreground 
                            : &self->background;
        GLfloat *texpx = texture_idx (self, x + (7-b), y);

        *texpx++ = pixel->r;
        *texpx++ = pixel->g;
        *texpx++ = pixel->b;
    }

    if (self->tex_sync)
    {
        self->sub_start.x = x;
        self->sub_start.y = y;
        self->sub_end.x = x+8;
        self->sub_end.y = y+1;
        self->tex_sync = false;
    }

    if (x < self->sub_start.x)
        self->sub_start.x = x;
    else if (x >= self->sub_end.x)
        self->sub_end.x = x+8;

    if (y < self->sub_start.y)
        self->sub_start.y = y;
    else if (y >= self->sub_end.y)
        self->sub_end.y = y+1;
}

static void
crt_update_texture(crt_t *self)
{
    if (self->tex_sync)
        return;

    GLint row    = self->sub_start.y;
    GLint width  = self->sub_end.x - self->sub_start.x;
    GLint height = self->sub_end.y - self->sub_start.y;

    if (width == TX_WIDTH)
    {
        glBindTexture (GL_TEXTURE_2D, self->h_texture);
        glTexSubImage2D (
                GL_TEXTURE_2D, 0,
                self->sub_start.x, self->sub_start.y,
                width, height,
                GL_RGB, GL_FLOAT, 
                texture_idx(self, self->sub_start.x, self->sub_start.y));
    }
    else
    {
        for (; row < self->sub_end.y; row++)
        {
            glBindTexture (GL_TEXTURE_2D, self->h_texture);
            glTexSubImage2D (
                    GL_TEXTURE_2D, 0,
                    self->sub_start.x, row,
                    width, 1,
                    GL_RGB, GL_FLOAT, 
                    texture_idx(self, self->sub_start.x, row));
        }
    }

    self->tex_sync = true;
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
