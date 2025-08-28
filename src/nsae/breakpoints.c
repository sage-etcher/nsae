
#include "breakpoints.h"

#include "log.h"

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

    self->m[self->cnt++] = pc;
    return 0;
}

int
br_remove (breakpoints_t *self, uint16_t pc)
{
    assert (self != NULL);
    int index = br_lookup (self, pc);
    if (index == -1) return -1;
    uint16_t addr = self->m[index];

    memcpy (&self->m[index], &self->m[index+1], --self->cnt - index);
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


/* end of file */
