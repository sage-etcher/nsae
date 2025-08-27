
#include "nsae.h"

#include "adv.h"
#include "crt.h"
#include "kb.h"
#include "log.h"
#include "nsaeipc.h"
#include "server.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


bool g_log_info = true;
bool g_log_verbose = true;
bool g_log_debug = true;
bool g_log_warning = true;
bool g_log_error = true;

static int gl_init (float win_width, float win_height,
                    float emu_width, float emu_height);

static void nsae_reshape (int width, int height, void *cb_data);
static void nsae_key_handler (unsigned char key, int x, int y, void *cb_data);
static void nsae_update (void *cb_data);
static void nsae_render (void *cb_data);
static void nsae_main_loop (int val, void *cb_data);

static int
gl_init (float win_width, float win_height, float emu_width, float emu_height)
{
    GLenum glew_error = glewInit ();
    if (glew_error != GLEW_OK)
    {
        log_error ("nsae: glew: initialization error: %s\n",
                glewGetErrorString (glew_error));
        return 1;
    }

    if (!GLEW_VERSION_1_2)
    {
        log_error ("nsae: glew: insuffient OpenGL version, requires >=1.2\n");
        return 2;
    }

    glViewport (0.f, 0.f, win_width, win_height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0.f, emu_width, emu_height, 0.f, 1.f, -1.f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glClearColor (0.f, 0.f, 0.f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT);
    glutSwapBuffers ();

    GLenum gl_error = glGetError ();
    if (gl_error != GL_NO_ERROR)
    {
        log_error ("nsae: opengl: initialization error: %s\n",
                gluErrorString (gl_error));
        return 1;
    }

    return 0;
}

int
nsae_start (nsae_t *self, int *p_argc, char **argv)
{
    assert (self != NULL);

    /* initialize emulator */
    self->width  = 640;
    self->height = 480;
    self->max_fps = 60;

    self->pause = true;
    self->exit = false;

    int rc = 0;
    rc |= nsae_ipc_init (NSAE_IPC_SERVER, NULL, NULL);
    rc |= adv_init (&self->adv);

    if (rc != 0)
    {
        log_error ("nsae: failed to initialize\n");
        return 1;
    }

    /* initialize freeglut */
    glutInit (p_argc, argv);
    glutInitContextVersion (1, 2);

    /* create glut window */
    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (self->width, self->height);
    glutCreateWindow ("NorthStar Advantage Emulator");

    /* TODO: initialize freeglut menu */

    /* initialize opengl */
    self->width  = glutGet (GLUT_WINDOW_WIDTH);
    self->height = glutGet (GLUT_WINDOW_HEIGHT);

    rc = gl_init (self->width, self->height, 640, 240);
    if (rc != 0)
    {
        return 1;
    }

    /* initialize glut callbacks */
    glutReshapeFuncUcall (nsae_reshape, self);
    glutDisplayFuncUcall (nsae_render, self);
    glutKeyboardFuncUcall (nsae_key_handler, self);

    /* enter the main loop */
    glutTimerFuncUcall (1000 / self->max_fps, nsae_main_loop, 0, self);
    glutMainLoop ();

    /* cleanup */
    nsae_ipc_free (NSAE_IPC_SERVER);

    return 0;
}

static void
nsae_reshape (int width, int height, void *cb_data)
{
    assert (cb_data != NULL);
    nsae_t *self = cb_data;

    glViewport (0.f, 0.f, width, height);
    self->width  = width;
    self->height = height;
}

static void
nsae_key_handler (unsigned char key, int x, int y, void *cb_data)
{
    assert (cb_data != NULL);
    nsae_t *nsae = cb_data;
    adv_t  *adv  = &nsae->adv;
    kb_t   *kb   = &adv->kb;

    //if (key == 'q')
    //{
    //    log_debug ("nsae: exit key pressed\n");
    //    glutLeaveMainLoop ();
    //    return;
    //}

    if (nsae->pause) return;

    uint8_t adv_key = kb_decode_key (kb, key);
    (void)kb_push (kb, adv_key);

    /* overflow */
    adv->stat2_reg |= kb->overflow << 5;

    /* maskable interupts */
    if (adv->kb_mi)
    {
        adv->stat2_reg |= kb->data_flag << 6;
    }

    /* non maskable intreupts */
    if (adv->kb_nmi)
    {
        adv->hw_interupt = kb->reset;
    }
}

static void
nsae_update (void *cb_data)
{
    assert (cb_data != NULL);
    nsae_t *self = cb_data;
    adv_t  *adv  = &self->adv;

    /* handle ipc */
    server_handle_ipc (self);

    /* emulate the system */
    if (self->pause) return;

    const int CPU_HZ = 4000000; /* 4MHz */
    int cycles = CPU_HZ / self->max_fps;
    (void)adv_run (adv, cycles);
}

static void
nsae_render (void *cb_data)
{
    assert (cb_data != NULL);
    nsae_t *nsae = cb_data;
    adv_t *adv = &nsae->adv;
    crt_t *crt = &adv->crt;

    glClear (GL_COLOR_BUFFER_BIT);

    glBegin (GL_QUADS);
        crt_draw (crt);
    glEnd ();

    glutSwapBuffers ();

    if (adv->crt_mi)
    {
        adv->stat1_reg |= crt->vrefresh << 2;
    }
}

static void
nsae_main_loop (int val, void *cb_data)
{
    assert (cb_data != NULL);
    nsae_t *self = cb_data;

    if (self->exit)
    {
        log_verbose ("nsae: exit conditional handled\n");
        glutLeaveMainLoop ();
        return;
    }

    /* queue the next iteration imediately */
    glutTimerFuncUcall (1000 / self->max_fps, nsae_main_loop, val, self);

    nsae_update (self);
    nsae_render (self);

}

/* end of file */
