
#include "mmu.h"
#include "nsae.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <assert.h>
#include <stdio.h>


void
reshape (int width, int height, void *data)
{
    adv_t *self = data;
    assert (self != NULL);

    self->win_width = width;
    self->win_height = height;
    glViewport (0.f, 0.f, width, height);
}


void
update (void *data)
{
    adv_t *self = data;
    assert (self != NULL);

    if (self->emu_paused) return;

    unsigned adv_cpu_speed = 4000000;
    unsigned adv_cycles = adv_cpu_speed / self->win_max_fps;
    (void)adv_run (self, adv_cycles);
}

void
key_handler (unsigned char key, int x, int y, void *data)
{
    adv_t *self = data;
    assert (self != NULL);
    /* handle emulator reserved */
    if (key == 'q')
    {
        self->win_exit = 1;
    }

    /* pass throught to emulator */

    if (self->emu_paused) return;

    /* handle keyboard buffer overrun bit */
    if (self->kb_count >= ADV_KB_BUF_SIZE)
    {
        self->status2_reg |= 0x20;
        return;
    }

    /* load kb data into kb buffer */
    self->kb_buf[self->kb_count++] = key;

    /* set keyboard data flag */
    if (self->kb_mi)
    {
        self->status2_reg |= 0x40;
    }
}

void
menu_handler (int value, void *data)
{
    adv_t *self = data;
    assert (self != NULL);

    switch (value)
    {
    case 1: self->win_exit = 1; break; /* exit */
    case 2: self->emu_paused = 1; break; /* pause */
    case 3: self->emu_paused = 0; break; /* start */

    case 4: /* disk B: eject */
    case 5: /* disk B: load */
    case 6: /* disk A: eject */
    case 7: /* disk A: load */
            fprintf (stderr, "nsae: debug: unimplemented menu entry\n");
            break;
    }

    return;
}

void
render (void *data)
{
    adv_t *self = data;
    assert (self != NULL);

    glClear (GL_COLOR_BUFFER_BIT);
    
    /* if blank display, dont update */
    if (!(self->control_reg & 0x20)) 
    {
        glBegin (GL_QUADS);

            glColor3f (0.f, 1.f, 0.f);

            for (int y = 0; y < 240 ; y++)
            {
                for (int x = 0; x < 80; x++)
                {
                    uint32_t offset = x * 0x100 + y + self->scroll_reg;
                    unsigned char *p = &self->memory[0x10000 + offset];
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
    }

    glutSwapBuffers ();


    /* set display flag */
    if (self->control_reg & 0x80)
    {
        self->status1_reg |= 0x04;
    }
}

void
main_loop (int val, void *data)
{
    adv_t *self = data;
    assert (self != NULL);

    /* exit before doing anyting if it is time */
    if (self->win_exit) 
    {
        glutLeaveMainLoop ();
        return;
    }

    /* queue the next iteration imediately */
    glutTimerFuncUcall (1000 / self->win_max_fps, main_loop, val, self);

    update (self);
    render (self);
}

int
init_gl (adv_t *self)
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

    glViewport (0.f, 0.f, self->win_width, self->win_height);

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
    /* initialize emulator state */
    adv_t emu = { 0 };
    adv_t *self = &emu;
    adv_init (self);

    /* initialize freeglut */
    glutInit (&argc, argv);
    glutInitContextVersion (1, 2);

    /* create glut window */
    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (self->win_width, self->win_height);
    glutCreateWindow ("NorthStar Advantage Emulator");

    self->win_width  = glutGet (GLUT_WINDOW_WIDTH);
    self->win_height = glutGet (GLUT_WINDOW_HEIGHT);

    /* create glut menu */
    int floppy_a_menu = glutCreateMenuUcall (menu_handler, self);
    glutAddMenuEntry ("eject A", 7);
    glutAddMenuEntry ("load disk file", 6);

    int floppy_b_menu = glutCreateMenuUcall (menu_handler, self);
    glutAddMenuEntry ("eject B", 5);
    glutAddMenuEntry ("load disk file", 4);

    int root_menu = glutCreateMenuUcall (menu_handler, self);
    glutAddSubMenu ("floppy a", floppy_a_menu);
    glutAddSubMenu ("floppy b", floppy_b_menu);
    glutAddMenuEntry ("start", 3);
    glutAddMenuEntry ("pause", 2);
    glutAddMenuEntry ("exit", 1);
    glutAttachMenu (GLUT_RIGHT_BUTTON);

    /* initialize opengl context */
    if (init_gl (self))
    {
        fprintf (stderr, "nsae: cannot initialize OpenGL\n");
        return 1;
    }

    /* configure glut callback handlers */
    glutKeyboardFuncUcall (key_handler, self);
    glutReshapeFuncUcall (reshape, self);
    glutDisplayFuncUcall (render, self);

    glutTimerFuncUcall (1000 / self->win_max_fps, main_loop, 0, self);

    glutMainLoop ();

    /* clean up */
    adv_quit ();
    return 0;
}

/* end of file */
