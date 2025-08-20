
#ifndef NSAE_H
#define NSAE_H

#include "mmu.h"
#include "z80emu.h"

#include <stdint.h>

#define ADV_RAM (64+20+2) * (2<<10)
#define ADV_KB_BUF_SIZE 7

#define ADV_FD_COUNT 2

#define ADV_FD_SIDES        2
#define ADV_FD_TRACK_CNT   35
#define ADV_FD_SECTOR_CNT  10
#define ADV_FD_BLKSIZE    512
#define ADV_FD_DATA_MAX 2*35*10*512

typedef struct {
    char *filename;
    uint8_t data[ADV_FD_DATA_MAX];
    uint8_t sector;
    uint8_t track;
} floppy_t;

enum {
    FD_NONE,
    FD_WRITE,
    FD_READ,
};

typedef struct {
    Z80_STATE cpu;
    mmu_t mmu;
    uint8_t memory[ADV_RAM];

    int win_width;
    int win_height;
    int win_max_fps;
    int win_exit;

    int emu_paused;

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

    uint8_t fdc_reg;
    uint8_t fd_num;
    uint8_t fd_mode;
    uint8_t fd_first_read;
    uint16_t fd_read_cnt;
    floppy_t fd[ADV_FD_COUNT];
} adv_t;

int adv_init (adv_t *self);
void adv_quit (void);

int adv_run (adv_t *self, int number_cycles);

uint8_t adv_read  (adv_t *self, uint16_t addr, Z80_STATE *state);
void    adv_write (adv_t *self, uint16_t addr, uint8_t data, Z80_STATE *state);

uint8_t adv_in  (adv_t *self, uint8_t port, Z80_STATE *state);
void    adv_out (adv_t *self, uint8_t port, uint8_t data, Z80_STATE *state);

#endif
/* end of file */
