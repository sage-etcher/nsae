
#include "lutil.h"

int g_screen_width  = 640;
int g_screen_height = 480;
int g_screen_fps = 60;


void main_loop (int val);
/* Pre Condition:
 *  -Initialized freeGLUT
 * Post Condition:
 *  -Calls the main loop fuctions and sets intself to be called back in
 *   1000 / g_screen_fps milliseconds
 * Side Effects:
 *  -Sets glutTimerFunc */

int
main (int argc, char **argv)
{
    /* initialize FreeGLUT */
    glutInit (&argc, argv);
    glutInitContextVersion (2, 1);

    /* create double buffered window */
    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (g_screen_width, g_screen_height);
    glutCreateWindow ("OpenGL");

    /* initialize glew */
    GLenum res = glewInit ();
    if (res != GLEW_OK)
    {
        fprintf (stderr, "nsae: error unable to initialize loader library: %d: %s\n",
                res, glewGetErrorString (res));
        exit (EXIT_FAILURE);
    }

    if (!GLEW_VERSION_2_1)
    {
        fprintf (stderr, "nsae: error missing support for OpenGL 2.1\n");
        exit (EXIT_FAILURE);
    }

    /* do post window/context creation initialization */
    if (init_gl ())
    {
        fprintf (stderr, "nsae: error unable to initialize graphics library\n");
        exit (EXIT_FAILURE);
    }

    /* load media */
    if (load_media ())
    {
        fprintf (stderr, "nsae: error unable to load media\n");
        exit (EXIT_FAILURE);
    }

    /* set rendering function */
    glutDisplayFunc (render);

    /* set the main loop */
    glutTimerFunc (1000 / g_screen_fps, main_loop, 0);

    /* start glut main loop */
    glutMainLoop ();

    exit (EXIT_SUCCESS);
}

void
main_loop (int val)
{
    /* frame logic */
    update ();
    render ();

    /* run frame one more time */
    glutTimerFunc (1000 / g_screen_fps, main_loop, val);
}

/* end of file */
