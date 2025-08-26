
#include "server.h"

#include "adv.h"
#include "fdc.h"
#include "nsae.h"
#include "nsaecmd.h"
#include "nsaeipc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
server_handle_ipc (nsae_t *self)
{
    uint8_t cmd = 0x00;
    uint8_t fd_num = 0x00;
    uint32_t abs_addr = 0x00000000;
    char *file = NULL;
    size_t n = 0;

    int rc = nsae_ipc_recieve (&cmd, sizeof (uint8_t));
    if (rc == 0) return; /* non blocking */

    /* get parameters */
    switch (cmd)
    {
    case NSAE_CMD_BREAKPOINT:
        while (nsae_ipc_recieve ((uint8_t *)&abs_addr, sizeof (uint32_t)) == 0) { }
        break;

    case NSAE_CMD_EJECT_FD:
        while (nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t)) == 0) { }
        break;

    case NSAE_CMD_LOAD_FD:
    case NSAE_CMD_SAVE_FD:
        while (nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t)) == 0) { }
    case NSAE_CMD_LOAD_HD:
    case NSAE_CMD_SAVE_HD:
        while (nsae_ipc_recieve ((uint8_t *)&n, sizeof (size_t)) == 0) { }
        file = malloc (n+1);
        while (nsae_ipc_recieve ((uint8_t *)file, n) == 0) { }
        file[n] = '\0';
        break;
    }

    /* execute logic */
    switch (cmd)
    {
    case NSAE_CMD_BREAKPOINT:
        printf ("breakpiont at %05x\n", abs_addr);
        break;

    case NSAE_CMD_CONTINUE: self->pause = false; break;

    case NSAE_CMD_EJECT_FD:
        printf ("fd_eject %1d\n", fd_num);
        break;

    case NSAE_CMD_EJECT_HD:
        printf ("hd_eject\n");
        break;

    case NSAE_CMD_EXIT: self->exit = true; break;
    case NSAE_CMD_LOAD_FD: fdc_load_disk (&self->adv.fdc, fd_num, file); break;

    case NSAE_CMD_LOAD_HD:
        printf ("hd_load %s\n", file);
        break;

    case NSAE_CMD_PAUSE: self->pause = true; break;

    case NSAE_CMD_RESTART:
        puts ("restart\n");
        break;

    case NSAE_CMD_RUN: self->step = false; break;
    case NSAE_CMD_SAVE_FD: fdc_save_disk (&self->adv.fdc, fd_num, file); break;

    case NSAE_CMD_SAVE_HD:
        printf ("hd_save %s\n", file);
        break;

    case NSAE_CMD_STATUS:
        printf ("status: %d %d\n", self->pause, self->exit);
        break;

    case NSAE_CMD_STEP: 
        self->step = true;
        self->step_pulse = true; 
        break;
    }

    free (file);
    return;
}



/* end of file */
