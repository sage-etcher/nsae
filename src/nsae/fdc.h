
#ifndef NSAE_FDC_H
#define NSAE_FDC_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

#define FD_CNT       2
#define FD_SIDES     2
#define FD_TRACKS   35
#define FD_SECTORS  10
#define FD_BLKSIZE 512

#define FD_SSDD_SIZE 179200
#define FD_DSDD_SIZE 358400
#define FD_MAX_SIZE  FD_DSDD_SIZE

#define FD_SSDD 0
#define FD_DSDD 1

#define FD_STEP_OUT 1
#define FD_STEP_IN  0

typedef struct {
    char *filename[FD_CNT];

    uint8_t data[FD_CNT][FD_MAX_SIZE];
    bool disk_type[FD_CNT];
    bool disk_loaded[FD_CNT];
    uint8_t preamble;
    uint8_t sync;
    uint16_t index;

    uint8_t sector[FD_CNT];
    uint8_t track[FD_CNT];
    uint8_t last_sector;

    bool powered;
    bool motor_enabled;

    bool disk;  /* 0:1 for disk A:B */
    bool side;  /* 0:1 for top:bottom */

    bool step_pulse;
    bool step_direction; /* 0:1 for out:in */
    bool track_zero;
    bool sector_mark;
    int  sector_mark_hold;
    bool serial_data;
    bool precompensation;

    bool read_mode;
    bool write_mode;
    bool hard_ro;

} fdc_t;


int fdc_init (fdc_t *self);
int fdc_load_disk (fdc_t *self, bool disk, char *filename);
int fdc_save_disk (fdc_t *self, bool disk, char *filename);
int fdc_eject (fdc_t *self, bool disk);

void fdc_reset (fdc_t *self);
void fdc_start_motor (fdc_t *self);
void fdc_stop_motor (fdc_t *self);

void fdc_load_drvctrl (fdc_t *self, uint8_t data);

void fdc_set_read (fdc_t *self, bool state);
void fdc_set_write (fdc_t *self, bool state);

void fdc_update (fdc_t *self);
void fdc_step (fdc_t *self);
void fdc_disk_rotate (fdc_t *self);
void fdc_next_sector (fdc_t *self);
uint8_t fdc_get_sector (fdc_t *self);

uint8_t fdc_read_sync1 (fdc_t *self);
uint8_t fdc_read_sync2 (fdc_t *self);
uint8_t fdc_read (fdc_t *self);

void fdc_write (fdc_t *self, uint8_t data);

uint32_t fdc_calc_disk_offset (uint8_t side, uint8_t track, uint8_t sector, 
        uint16_t i);

void fdc_status (fdc_t *self);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_FDC_H */
/* end of file */
