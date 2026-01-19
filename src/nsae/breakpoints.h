
#ifndef NSAE_BREAKPOINT_H
#define NSAE_BREAKPOINT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BREAKPOINT_MAX 256
typedef struct {
    uint16_t m[BREAKPOINT_MAX];
    size_t cnt;
} breakpoints_t;

int br_add (breakpoints_t *self, uint16_t pc);
int br_remove (breakpoints_t *self, uint16_t pc);
int br_lookup (breakpoints_t *self, uint16_t pc);
void br_list (breakpoints_t *self);

#endif /* NSAE_BREAKPOINT_H */
/* end of file */
