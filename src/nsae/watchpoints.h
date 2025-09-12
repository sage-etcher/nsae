
#ifndef NSAE_WATCHPOINTS_H
#define NSAE_WATCHPOINTS_H

#include "mmu.h"

#include <stddef.h>
#include <stdint.h>

enum {
    WP_UPDATE,
    WP_EQU,
    WP_NEQ,
    WP_GT,
    WP_GTE,
    WP_LT,
    WP_LTE,
    WP_MATCH_COUNT
};

enum {
    WP_U8,
    WP_U16,
    WP_TYPE_COUNT,
};

typedef struct {
    int match_method;
    int type;
    uint32_t addr;
    uint16_t value;
} watchpoint_t;

#define WATCHPOINT_MAX 256
typedef struct {
    watchpoint_t m[WATCHPOINT_MAX];
    size_t cnt;
} watchpoints_t;

int wp_set (watchpoints_t *self, uint32_t addr, uint16_t value, 
        int match_method, int type);

int wp_remove (watchpoints_t *self, size_t index);
int wp_lookup (watchpoints_t *self, uint32_t addr, mmu_t *p_mmu);
void wp_list (watchpoints_t *self);

#endif /* NSAE_WATCHPOINTS_H */
