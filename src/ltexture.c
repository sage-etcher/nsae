
#include "ltexture.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

ltexture_t *
ltex_new (void)
{
    ltexture_t *self = malloc (sizeof (ltexture_t));
    if (self == NULL) return NULL;

    self->id = 0;

    self->width = 0;
    self->height = 0;

    return self;
}

void
ltex_free (ltexture_t *self)
{
    if (self == NULL) return;

    if (self->id != 0)
    {
        glDeleteTextures (1, &self->id);
        self->id = 0;
    }

    self->width = 0;
    self->height = 0;

    free (self);
}

int
ltex_from_pixels32 (ltexture_t *self, GLuint *pixels,
                    GLuint width, GLuint height)
{
    assert (self != NULL);

    /* free texture if exists */
    ltex_free (self);

    /* set the texture dimentions */
    self->width = width;
    self->height = height;

    /* generate the texture id */
    glGenTextures (1, &self->id);

    /* bind texture id */
    glBindTexture (GL_TEXTURE_2D, self->id);

    /* generate the texture */
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, pixels);

    /* set texture parameters */
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* unbind texture */
    glBindTexture (GL_TEXTURE_2D, 0);

    /* check for error */
    GLenum error = glGetError ();
    if (error != GL_NO_ERROR)
    {
        fprintf (stderr, "nsae: error loading texture from %p pixels! %s\n",
                (void *)pixels, gluErrorString (error));
        return 1;
    }

    return 0;
}

void
ltex_render (ltexture_t *self, GLfloat x, GLfloat y)
{
    assert (self != NULL);

    /* skip if texture doesnt exist */
    if (self->id == 0) return;

    /* remove any previous transformations */
    glLoadIdentity ();

    /* move to rendering point */
    glTranslatef (x, y, 0.0f);

    /* set texture id */
    glBindTexture (GL_TEXTURE_2D, self->id);

    /* render textured quad */
    glBegin (GL_QUADS);
        glTexCoord2f (0.0f, 0.0f); glVertex2f (       0.0f,         0.0f);
        glTexCoord2f (1.0f, 0.0f); glVertex2f (self->width,         0.0f);
        glTexCoord2f (1.0f, 1.0f); glVertex2f (self->width, self->height);
        glTexCoord2f (0.0f, 1.0f); glVertex2f (       0.0f, self->height);
    glEnd ();
}


/* end of file */
