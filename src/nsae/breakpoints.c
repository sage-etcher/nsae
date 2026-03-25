
#define LOG_CATEGORY LC_NSAE
#include "breakpoints.h"

#include "nslog.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
br_add (breakpoints_t *self, uint16_t pc)
{
    assert (self != NULL);
    if (self->cnt >= BREAKPOINT_MAX)
    {
        log_error ("nsae: breakpoint: cannot set breakpoint, max %zu\n", 
                self->cnt);
        return 1;
    }

    /* break point already exists */
    if (-1 != br_lookup (self, pc)) 
    {
        return 0;
    }

    self->m[self->cnt++] = pc;
    return 0;
}

int
br_remove (breakpoints_t *self, size_t index)
{
    assert (self != NULL);
    if (index >= self->cnt) return -1;
    uint16_t addr = self->m[index];

    self->cnt--;
    self->m[index] = self->m[self->cnt];

    return addr;
}

int
br_lookup (breakpoints_t *self, uint16_t pc)
{
    assert (self != NULL);
    for (size_t i = 0; i < self->cnt; i++)
    {
        if (pc == self->m[i])
        {
            return (int)i;
        }
    }
    return -1;
}

void
br_list (breakpoints_t *self)
{
    log_info ("breakpoint list:\n");
    for (size_t i = 0; i < self->cnt; i++)
    {
        log_info ("%3lu: %04x\n", i, self->m[i]);
    }
}


/* end of file */
