
#ifndef NSAE_FDC_H
#define NSAE_FDC_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define FD_SIDES     2
#define FD_TRACKS   35
#define FD_SECTORS  10
#define FD_BLKSIZE 512

#define FD_MAX_SIZE 2*35*10*512

typedef struct {
    char *filename[2];

    size_t  data_cnt[2];
    uint8_t data[2][FD_MAX_SIZE];
    
    uint8_t sector[2];
    uint8_t track[2];

    bool powered;
    bool motor_enabled;

    bool disk; /* 0:1 for disk A:B */
    bool disk_side; /* 0:1 for top:bottom */

    bool step_pulse;
    bool step_direction; /* 0:1 for out:in */

    bool mode_read;
    bool mode_write;
    bool hard_ro;
} fdc_t;

int fdc_init (fdc_t *self);
int fdc_load_disk (bool disk, char *filename);
int fdc_save_disk (bool disk, char *filename);

void fdc_start_motor (fdc_t *self);
void fdc_stop_motor (fdc_t *self);

void fdc_step (fdc_t *self);
uint8_t fdc_get_sector (fdc_t *self);

uint8_t fdc_read_sync1 (fdc_t *self);
uint8_t fdc_read_sync2 (fdc_t *self);
uint8_t fdc_read (fdc_t *self);

void fdc_write (fdc_t *self, uint8_t data);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_FDC_H */
/* end of file */
