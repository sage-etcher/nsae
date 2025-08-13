
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <stdio.h>

static int s_screen_width  = 640;
static int s_screen_height = 480;
static int s_screen_fps = 60;

static unsigned char s_scroll_reg = 0;
static unsigned char s_vram[1024*20];
static size_t s_i = 0;

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

    /*         MVI     A,0F0h  ;7
     *         LXI     H,0     ;10
     * loop:   INX     H       ;5
     *         MOV     M,A     ;7
     *         JMP     loop    ;10
     * ;loop takes 22 cycles
     */

    for (int i = 0; i < adv_cycles; i += 22)
    {
        s_vram[s_i++] ^= 0b10000000;
        if (s_i >= sizeof (s_vram))
        {
            s_i = 0;
        }
    }



    s_vram[0x0104] = 0b00000000; s_vram[0x0204] = 0b11000000; s_vram[0x0304] = 0b00000000;
    s_vram[0x0105] = 0b00000001; s_vram[0x0205] = 0b01100000; s_vram[0x0305] = 0b00000000;
    s_vram[0x0106] = 0b00000011; s_vram[0x0206] = 0b00110000; s_vram[0x0306] = 0b00000000;
    s_vram[0x0107] = 0b00000110; s_vram[0x0207] = 0b00011000; s_vram[0x0307] = 0b00000000;
    s_vram[0x0108] = 0b00001100; s_vram[0x0208] = 0b00001100; s_vram[0x0308] = 0b00000000;
    s_vram[0x0109] = 0b00011111; s_vram[0x0209] = 0b11111110; s_vram[0x0309] = 0b00000000;
    s_vram[0x010a] = 0b00110000; s_vram[0x020a] = 0b00000011; s_vram[0x030a] = 0b00000000;
    s_vram[0x010b] = 0b01100000; s_vram[0x020b] = 0b00000001; s_vram[0x030b] = 0b10000000;
    s_vram[0x010c] = 0b11110000; s_vram[0x020c] = 0b00000011; s_vram[0x030c] = 0b11000000;

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
                size_t offset = x * 0x100 + y + s_scroll_reg;
                unsigned char *p = &s_vram[offset];
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
    update ();
    render ();

    glutTimerFunc (1000 / s_screen_fps, main_loop, val);
}

int
main (int argc, char **argv)
{
    glutInit (&argc, argv);
    glutInitContextVersion (1, 2);

    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (s_screen_width, s_screen_height);
    glutCreateWindow ("NorthStar Advantage Emulator");

    s_screen_width = glutGet (GLUT_WINDOW_WIDTH);
    s_screen_height = glutGet (GLUT_WINDOW_HEIGHT);

    GLenum glew_error = glewInit ();
    if (glew_error != GLEW_OK)
    {
        fprintf (stderr, "nsae: error loading glew: %s\n",
                glewGetErrorString (glew_error));
        return 1;
    }

    if (!GLEW_VERSION_1_2)
    {
        fprintf (stderr, "nsae: glew cannot find OpenGL 2.1\n");
        return 1;
    }

    glViewport (0.f, 0.f, s_screen_width, s_screen_height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0.f, 640, 240, 0.f, 1.f, -1.f);

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

    glutReshapeFunc (reshape);
    glutDisplayFunc (render);

    glutTimerFunc (1000 / s_screen_fps, main_loop, 0);

    glutMainLoop ();

    return 0;
}

/* end of file */
