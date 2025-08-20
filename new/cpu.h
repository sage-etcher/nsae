
#ifndef NSAE_CPU_H
#define NSAE_CPU_H
#ifdef __cplusplus
extern "c" {
#endif

#include "z80emu.h"

#include <stdbool.h>
#include <stdint.h>


typedef struct {
    Z80_STATE state;
} cpu_t;

int cpu_init (cpu_t *self);
void cpu_reset (cpu_t *self);
int cpu_run (cpu_t *self, int cycles, void *cb_data);
int cpu_step (cpu_t *self, void *cb_data);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_CPU_H */
/* end of file */
