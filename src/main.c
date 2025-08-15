
#include "mmu.h"
#include "nsae.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <stdio.h>

static int s_screen_width  = 640;
static int s_screen_height = 480;
static int s_screen_fps = 60;

static adv_t s_emu_state = { 0 };

static int s_exit_flag = 0;

void
reshape (int width, int height)
{
    s_screen_width = width;
    s_screen_height = height;
    glViewport (0.f, 0.f, s_screen_width, s_screen_height);
}


void
update (void)
{
    unsigned adv_cpu_speed = 4000000;
    unsigned adv_cycles = adv_cpu_speed / s_screen_fps;
    (void)adv_run (&s_emu_state, adv_cycles);
}

void
key_handler (unsigned char key, int x, int y)
{
    if (key == 'q')
    {
        s_exit_flag = 1;
    }
}

void
render (void)
{
    glClear (GL_COLOR_BUFFER_BIT);

    glBegin (GL_QUADS);

        glColor3f (0.f, 1.f, 0.f);

        for (int x = 0; x < 80; x++)
        {
            for (int y = 0; y < 240 ; y++)
            {
                uint32_t offset = x * 0x100 + y + s_emu_state.scroll_reg;
                unsigned char *p = &s_emu_state.memory[0x10000 + offset];
                for (int b = 0; b < 8; b++)
                {
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

    glEnd ();

    glutSwapBuffers ();
}

void
main_loop (int val)
{
    /* exit before doing anyting if it is time */
    if (s_exit_flag) 
    {
        glutLeaveMainLoop ();
        return;
    }

    /* queue the next iteration imediately */
    glutTimerFunc (1000 / s_screen_fps, main_loop, val);

    update ();
    render ();
}

int
init_gl (void)
{
    GLenum glew_error = glewInit ();
    if (glew_error != GLEW_OK)
    {
        fprintf (stderr, "nsae: error loading glew: %s\n",
                glewGetErrorString (glew_error));
        return 1;
    }

    if (!GLEW_VERSION_1_2)
    {
        fprintf (stderr, "nsae: glew cannot find OpenGL 1.2\n");
        return 1;
    }

    glViewport (0.f, 0.f, s_screen_width, s_screen_height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0.f, 640.f, 240.f, 0.f, 1.f, -1.f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glClearColor (0.f, 0.f, 0.f, 1.f);

    GLenum gl_error = glGetError ();
    if (gl_error != GL_NO_ERROR)
    {
        fprintf (stderr, "nsae: error loading OpenGL: %s\n",
                gluErrorString (gl_error));
        return 1;
    }

    return 0;
}

int
main (int argc, char **argv)
{
    /* initialize freeglut */
    glutInit (&argc, argv);
    glutInitContextVersion (1, 2);

    /* create glut window */
    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (s_screen_width, s_screen_height);
    glutCreateWindow ("NorthStar Advantage Emulator");

    s_screen_width = glutGet (GLUT_WINDOW_WIDTH);
    s_screen_height = glutGet (GLUT_WINDOW_HEIGHT);

    /* initialize opengl context */
    if (init_gl ())
    {
        fprintf (stderr, "nsae: cannot initialize OpenGL\n");
        return 1;
    }

    /* initialize emulator state */
    adv_init (&s_emu_state);

    /* configure glut callback handlers */
    glutKeyboardFunc (key_handler);
    glutReshapeFunc (reshape);
    glutDisplayFunc (render);

    glutTimerFunc (1000 / s_screen_fps, main_loop, 0);

    glutMainLoop ();

    /* clean up */
    adv_quit ();
    return 0;
}

/* end of file */
