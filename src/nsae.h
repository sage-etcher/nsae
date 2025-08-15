
#ifndef NSAE_H
#define NSAE_H

#include "mmu.h"
#include "z80emu.h"

#include <stdint.h>

#define ADV_RAM (64+20+2) * (2<<10)
#define ADV_KB_BUF_SIZE 7

#define ADV_FDISK_COUNT 2

#define ADV_FDISK_SIDES        2
#define ADV_FDISK_TRACK_CNT   35
#define ADV_FDISK_SECTOR_CNT  10
#define ADV_FDISK_BLKSIZE    512
#define ADV_FDISK_DATA_MAX 2*35*10*512

typedef struct {
    char *filename;
    uint8_t side;
    uint8_t track;
    uint8_t sector;
    uint16_t blksize;
    uint8_t read_only;
    uint8_t data[ADV_FDISK_DATA_MAX];
} fcu_t;

typedef struct {
    Z80_STATE cpu;
    mmu_t mmu;
    uint8_t memory[ADV_RAM];

    uint8_t scroll_reg;
    uint8_t control_reg;
    uint8_t status1_reg;
    uint8_t status2_reg;

    uint8_t cursor_lock;
    uint8_t kb_caps;
    uint8_t kb_mi;
    uint8_t kb_nmi;
    uint8_t kb_buf[ADV_KB_BUF_SIZE];
    uint8_t kb_count;

    uint8_t motor_enable;
    uint8_t disk_select;
    fcu_t floppys[ADV_FDISK_COUNT];
} adv_t;

int adv_init (adv_t *self);
void adv_quit (void);

int adv_run (adv_t *self, int number_cycles);

uint8_t adv_read  (adv_t *self, uint16_t addr);
void    adv_write (adv_t *self, uint16_t addr, uint8_t data);

uint8_t adv_in  (adv_t *self, uint8_t port);
void    adv_out (adv_t *self, uint8_t port, uint8_t data);

#endif
/* end of file */
