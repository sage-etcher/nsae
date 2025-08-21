
#ifndef NSAE_FDC_H
#define NSAE_FDC_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FD_SIDES     2
#define FD_TRACKS   35
#define FD_SECTORS  10
#define FD_BLKSIZE 512

#define FD_SSDD_SIZE 179200
#define FD_DSDD_SIZE 358400
#define FD_MAX_SIZE  FD_DSDD_SIZE

#define FD_SSDD 0
#define FD_DSDD 1

#define FD_STEP_OUT 0
#define FD_STEP_IN  1

typedef struct {
    char *filename[2];

    uint8_t data[2][FD_MAX_SIZE];
    bool disk_type[2];
    uint8_t preamble;
    uint8_t sync;
    uint16_t index;

    uint8_t sector[2];
    uint8_t track[2];

    bool powered;
    bool motor_enabled;

    bool disk;  /* 0:1 for disk A:B */
    bool side;  /* 0:1 for top:bottom */

    bool track_zero;
    bool step_pulse;
    bool step_direction; /* 0:1 for out:in */
    bool precompensation;

    bool read_mode;
    bool write_mode;
    bool hard_ro;
} fdc_t;


int fdc_init (fdc_t *self);
int fdc_load_disk (fdc_t *self, bool disk, char *filename);
int fdc_save_disk (fdc_t *self, bool disk, char *filename);

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
