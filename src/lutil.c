
#include "ltexture.h"
#include "lutil.h"

#include <stddef.h>
#include <stdio.h>


static ltexture_t *s_checkerboard_tex = NULL;

int
init_gl (void)
{
    /* set the viewport */
    glViewport (0.0f, 0.0f, g_screen_width, g_screen_height);

    /* initialize projection matrix */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0.0, g_screen_width, g_screen_height, 0.0, 1.0, -1.0);

    /* initialize modelview matrix */
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    /* initialize the clear color */
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

    /* enable texturing */
    glEnable (GL_TEXTURE_2D);

    /* check for error */
    GLenum error = glGetError ();
    if (error != GL_NO_ERROR)
    {
        fprintf (stderr, "nsae: error initializing OpenGL: %d: %s\n",
                error, gluErrorString (error));
        return 1;
    }

    return 0;
}

int
load_media (void)
{
    /* checkerboard pixels */
    enum {
        CHECKERBOARD_WIDTH = 128,
        CHECKERBOARD_HEIGHT = 128,
        CHECKERBOARD_PIXEL_COUNT = CHECKERBOARD_WIDTH * CHECKERBOARD_HEIGHT,
    };
    GLuint checkerboard[CHECKERBOARD_PIXEL_COUNT];

    /* go through pixels */ for (int i = 0; i < CHECKERBOARD_PIXEL_COUNT; i++) {
        GLubyte *colors = (GLubyte *)&checkerboard[i];
        GLubyte a = (i & 0x01) ^ ((i & 0x80) >> 7);

        colors[0] = 0xFF;
        colors[1] = 0xFF * a;
        colors[2] = 0xFF * a;
        colors[3] = 0xFF;
    }

    /* allocate the texture */
    if (s_checkerboard_tex == NULL)
    {
        s_checkerboard_tex = ltex_new ();
        if (s_checkerboard_tex == NULL)
        {
            fprintf (stderr, "nsae: error creating checkerboard texture\n");
            return 1;
        }
    }

    /* load the texture */
    if (ltex_from_pixels32 (s_checkerboard_tex, checkerboard, 
                CHECKERBOARD_WIDTH, CHECKERBOARD_HEIGHT))
    {
        fprintf (stderr, "nsae: unable to load checkerboard texture!\n");
        return 1;
    }

    return 0;
}

void
update (void)
{
    return;
}

void
render (void)
{
    glClear (GL_COLOR_BUFFER_BIT);

    GLfloat x = (g_screen_width - s_checkerboard_tex->width) / 2.0f;
    GLfloat y = (g_screen_height - s_checkerboard_tex->height) / 2.0f;

    ltex_render (s_checkerboard_tex, x, y);

    glutSwapBuffers ();
}


/* end of file */
