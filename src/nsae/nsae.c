
#define LOG_CATEGORY LC_NSAE
#include "nsae.h"

#include "adv.h"
#include "crt.h"
#include "kb.h"
#include "nsaeipc.h"
#include "nslog.h"
#include "server.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

static int gl_init (GLFWwindow *win, float win_width, float win_height,
                                     float emu_width, float emu_height);

static void nsae_key_handler (GLFWwindow *win, int key, int scan, int action, 
        int mods);
static void nsae_update (GLFWwindow *win);
static void nsae_render (GLFWwindow *win);

static int
gl_init (GLFWwindow *win, float win_width, float win_height, 
        float emu_width, float emu_height)
{
    GLenum glew_error = glewInit ();
    if (glew_error != GLEW_OK)
    {
        log_fatal ("nsae: glew: initialization error: %s\n",
                glewGetErrorString (glew_error));
        return 1;
    }

    if (!GLEW_VERSION_1_2)
    {
        log_fatal ("nsae: glew: insuffient OpenGL version, requires >=1.2\n");
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
    glfwSwapBuffers (win);

    GLenum gl_error = glGetError ();
    if (gl_error != GL_NO_ERROR)
    {
        log_fatal ("nsae: opengl: initialization error: %s\n",
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
    self->scale_multiplier = 1.0f;
    self->cycle_multiplier = 20.0f;

    self->width  = 640.0 * self->scale_multiplier;
    self->height = 480.0 * self->scale_multiplier;
    self->max_fps = 60;

    self->pause = true;
    self->exit = false;

    int rc = 0;
    log_init (LC_COUNT);
    rc |= nsae_ipc_init (NSAE_IPC_SERVER, NULL, NULL);
    rc |= adv_init (&self->adv);

    if (rc != 0)
    {
        log_fatal ("nsae: failed to initialize\n");
        return 1;
    }

    /* initialize glfw */
    if (!glfwInit ())
    {
        log_fatal ("nsae: glfw: initialization failed\n");
        return 1;
    }

    /* create glfw window */
    GLFWwindow *win = NULL;
    win = glfwCreateWindow (
            self->width, self->height,
            "NorthStar Advantage Emulator",
             NULL, NULL);
    if (win == NULL)
    {
        log_fatal ("nsae: glfw: window creation failed\n");
        return 1;
    }
    glfwMakeContextCurrent (win);

    /* initialize opengl */
    glfwGetWindowSize (win, &self->width, &self->height);

    rc = gl_init (win, self->width, self->height, 640, 240);
    if (rc != 0) { return 1; }

    /* initialize glfw callbacks */
    glfwSetKeyCallback (win, nsae_key_handler);
    glfwSetWindowUserPointer (win, self);

    /* enter the main loop */
    while (!glfwWindowShouldClose (win) && !self->exit)
    {
        glfwGetWindowSize (win, &self->width, &self->height);
        glViewport (0, 0, self->width, self->height);
        glClear (GL_COLOR_BUFFER_BIT);

        /* process input */
        nsae_update (win);

        /* render output */
        nsae_render (win);

        glfwSwapBuffers (win);
        glfwPollEvents ();
    }

    /* cleanup */
    glfwMakeContextCurrent (NULL);
    glfwDestroyWindow (win);
    glfwTerminate ();

    nsae_ipc_free (NSAE_IPC_SERVER);
    log_quit ();

    return 0;
}

static uint8_t
nsae_key_decode (int key, int mods)
{
    return key;
}

static void
nsae_key_handler (GLFWwindow *win, int key, int scan, int action, int mods)
{
    nsae_t *nsae = glfwGetWindowUserPointer (win);
    adv_t  *adv  = &nsae->adv;
    kb_t   *kb   = &adv->kb;

    if (nsae->pause) return;

    adv->kb.autorepeat = (action == GLFW_REPEAT);

    uint8_t adv_key = nsae_key_decode (key, mods);
    if (action == GLFW_PRESS)
    {
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

}

static void
nsae_update (GLFWwindow *win)
{
    nsae_t *self = glfwGetWindowUserPointer (win);
    adv_t  *adv  = &self->adv;

    /* handle ipc */
    server_handle_ipc (self);

    /* emulate the system */
    if (self->pause) return;

    unsigned long CPU_HZ = 4000000; /* 4MHz */
    unsigned long cycles = self->cycle_multiplier * (CPU_HZ / self->max_fps) ;
    unsigned long i = 0;

    if (self->step)
    {
        cycles = 1;
        self->pause = true;
    }

    i += adv_run (adv, cycles, self);
    if (self->pause)
    {
        cpu_status (&adv->cpu, &adv->mmu);
    }
}

static void
nsae_render (GLFWwindow *win)
{
    nsae_t *self = glfwGetWindowUserPointer (win);
    adv_t *adv = &self->adv;
    crt_t *crt = &adv->crt;

    glClear (GL_COLOR_BUFFER_BIT);

    glBegin (GL_QUADS);
        crt_draw (crt);
    glEnd ();

    glfwSwapBuffers (win);

    if (adv->crt_mi)
    {
        adv->stat1_reg |= crt->vrefresh << 2;
    }
}

void
nsae_status (nsae_t *self)
{
    log_info ("step: %d\n", self->step);
    log_info ("pause: %d\n", self->pause);
}

/* end of file */
