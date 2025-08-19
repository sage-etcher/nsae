
#ifndef NSAE_ADV_H
#define NSAE_ADV_H
#ifdef __cplusplus
extern "c" {
#endif

#include "cpu.h"
#include "crt.h"
#include "fdc.h"
#include "io.h"
#include "kb.h"
#include "mmu.h"
#include "ram.h"

#include <stdbool.h>
#include <stdint.h>


typedef struct {
    cpu_t cpu;
    crt_t crt;
    fdc_t fdc;
    io_t  io;
    kb_t  kb;
    mmu_t mmu;
    ram_t ram;
    speaker_t speaker;

    bool hw_interupt;

    bool cmd_ack;
    uint8_t ctrl_reg;
    uint8_t stat1_reg;
    uint8_t stat2_reg;
} adv_t;

int adv_init (adv_t *self);

uint8_t adv_in (adv_t *self, uint8_t port)
void adv_out (adv_t *self, uint8_t port, uint8_t data)


#ifdef __cplusplus
}
#endif
#endif /* NSAE_ADV_H */
/* end of file */
