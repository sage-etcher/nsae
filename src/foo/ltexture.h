
#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <stdio.h>

#include "lopengl.h"


typedef struct {
    GLuint id;
    GLuint width;
    GLuint height;
} ltexture_t;

ltexture_t ltex_new (void);
/* Post Condition:
 *  -Initializess member variables */

void ltex_free (ltexture_t *self);
/* Post Condition:
 *  -Frees texture */

int ltex_from_pixels32 (ltexture_t *self, GLuint *pixels, 
        GLuint width, GLuint height);
/* Pre Condition:
 *  -A valid OpenGL context
 * Post Condition:
 *  -Creates a texture rom th given pixels
 *  -Reports error to console if texture could not be created
 * Side Effects:
 *  -Binds a NULL texture */

void ltex_render (ltexture_t *self, GLfloat x, GLfloat y);
/* Pre Condition:
 *  -A valid OpenGL context
 *  -Active modelview matrix
 * Post Condition:
 *  -Translates to given position and renders textured quad
 * Side Effects:
 *  -Binds member texture ID */

#endif
/* end of file */
