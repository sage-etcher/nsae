
#include "server.h"

#include "adv.h"
#include "fdc.h"
#include "log.h"
#include "nsae.h"
#include "nsaecmd.h"
#include "nsaeipc.h"
#include "ram.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
server_handle_ipc (nsae_t *self)
{
    uint8_t cmd = 0x00;
    uint8_t fd_num = 0x00;
    uint32_t abs_addr = 0x00000000;
    char *file = NULL;
    size_t n = 0;

    int rc = nsae_ipc_recieve (&cmd, sizeof (uint8_t));
    if (rc == 0) return 0; /* non blocking */

    /* get parameters */
    switch (cmd)
    {
    case NSAE_CMD_BREAKPOINT:
        while (nsae_ipc_recieve ((uint8_t *)&abs_addr, sizeof (uint32_t)) == 0) { }
        break;

    case NSAE_CMD_FD_EJECT:
        while (nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t)) == 0) { }
        break;

    case NSAE_CMD_FD_LOAD:
    case NSAE_CMD_FD_SAVE:
        while (nsae_ipc_recieve ((uint8_t *)&fd_num, sizeof (uint8_t)) == 0) { }
    case NSAE_CMD_HD_LOAD:
    case NSAE_CMD_HD_SAVE:
        while (nsae_ipc_recieve ((uint8_t *)&n, sizeof (size_t)) == 0) { }
        file = malloc (n+1);
        while (nsae_ipc_recieve ((uint8_t *)file, n) == 0) { }
        file[n] = '\0';
        break;
    }

    if (fd_num >= FD_CNT)
    {
        log_error ("nsae: server: fd_num out of range 0-1 -- %u\n", fd_num);
        free (file);
        return 1;
    }

    if (abs_addr >= RAM_SIZE)
    {
        log_error ("nsae: server: abs_addr out of range 0-%u -- %u\n", 
                RAM_SIZE, abs_addr);
        free (file);
        return 1;
    }

    /* execute logic */
    switch (cmd)
    {
    /* floppy */
    case NSAE_CMD_FD_EJECT:
        log_verbose ("nsae: server: fd_eject %d\n", fd_num);
        fdc_eject (&self->adv.fdc, fd_num);
        break;

    case NSAE_CMD_FD_LOAD:
        log_verbose ("nsae: server: fd_load %d %s\n", fd_num, file);
        fdc_load_disk (&self->adv.fdc, fd_num, file);
        break;

    case NSAE_CMD_FD_SAVE:
        log_verbose ("nsae: server: fd_save %d %s\n", fd_num, file);
        fdc_save_disk (&self->adv.fdc, fd_num, file);
        break;

    /* harddisk */
    case NSAE_CMD_HD_EJECT:
        log_verbose ("nsae: server: hd_eject\n");
        break;

    case NSAE_CMD_HD_LOAD:
        log_verbose ("nsae: server: hd_load %s\n", file);
        break;

    case NSAE_CMD_HD_SAVE:
        log_verbose ("nsae: server: hd_save %s\n", file);
        break;

    /* emulator */
    case NSAE_CMD_BREAKPOINT:
        log_verbose ("nsae: server: breakpiont %05x\n", abs_addr);
        break;

    case NSAE_CMD_CONTINUE:
        log_verbose ("nsae: server: continue\n");
        self->pause = false;
        break;

    case NSAE_CMD_EXIT:
        log_verbose ("nsae: server: exit\n");
        self->exit = true;
        break;

    case NSAE_CMD_PAUSE:
        log_verbose ("nsae: server: pause\n");
        self->pause = true;
        break;

    case NSAE_CMD_RESTART:
        log_verbose ("nsae: server: restart\n");
        break;

    case NSAE_CMD_RUN:
        log_verbose ("nsae: server: run\n");
        self->step = false;
        break;

    case NSAE_CMD_STATUS:
        log_verbose ("nsae: server: status\n");
        log_info ("status: %d %d\n", self->pause, self->exit);
        break;

    case NSAE_CMD_STEP: 
        log_verbose ("nsae: server: step\n");
        self->step = true;
        self->step_pulse = true; 
        break;
    }

    free (file);
    return 0;
}



/* end of file */
