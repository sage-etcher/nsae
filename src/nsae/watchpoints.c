
#define LOG_CATEGORY LC_WATCHPOINTS
#include "watchpoints.h"

#include "mmu.h"
#include "nslog.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

int
wp_set (watchpoints_t *self, uint32_t addr, uint16_t value, int match_method,
        int type)
{
    if (self == NULL) return -1;
    if (self->cnt >= WATCHPOINT_MAX) return -1;
    if (match_method >= WP_MATCH_COUNT) return -1;
    if (type >= WP_TYPE_COUNT) return -1;

    size_t index = self->cnt;

    self->m[index] = (watchpoint_t){
        .match_method = match_method,
        .type  = type,
        .addr  = addr,
        .value = value,
    };
    self->cnt++;

    return index;
}

int
wp_remove (watchpoints_t *self, size_t index)
{
    if (self == NULL) return -1;
    if (index >= self->cnt) return -1;

    memcpy (&self->m[index], &self->m[index+1], self->cnt - index);
    self->cnt--;

    return 0;
}

int
wp_lookup (watchpoints_t *self, uint32_t addr, mmu_t *p_mmu)
{
    if (self == NULL) return 0;

    int exit_status = 0;

    size_t i = 0;
    for (i = 0; i < self->cnt; i++)
    {
        watchpoint_t *iter = &self->m[i];

        /* check addr */
        if ((iter->addr != addr) ||
            ((iter->type == WP_U16) && (iter->addr+1 != addr)))
        {
            continue;
        }

        /* get data */
        uint16_t data = mmu_read (p_mmu, iter->addr);
        if (iter->type == WP_U16)
        {
            data |= mmu_read (p_mmu, iter->addr+1) << 8;
        }

        /* data comparison */
        switch (iter->match_method)
        {
        case WP_UPDATE: exit_status |= 1; break;
        case WP_EQU:    exit_status |= (data == iter->value); break;
        case WP_NEQ:    exit_status |= (data != iter->value); break;
        case WP_GT:     exit_status |= (data >  iter->value); break;
        case WP_GTE:    exit_status |= (data >= iter->value); break;
        case WP_LT:     exit_status |= (data <  iter->value); break;
        case WP_LTE:    exit_status |= (data <= iter->value); break;

        default:
            log_error ("nsae: watchpoint: unkown match_method %02x\n",
                    iter->match_method);
            continue;
        }
    }

    return exit_status;
}

void
wp_list (watchpoints_t *self)
{
    if (self == NULL) return;

    size_t i = 0;
    for (i = 0; i < self->cnt; i++)
    {
        log_info ("%3d: (%05x) %04x %2x %2x\n",
                i,
                self->m[i].addr,
                self->m[i].value,
                self->m[i].match_method,
                self->m[i].type);
    }
}

/* end of file */
