
#define LOG_CATEGORY LC_FDC
#include "fdc.h"

#include "nslog.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int
fdc_init (fdc_t *self)
{
    assert (self != NULL);

    memset (self, 0, sizeof (*self));
    self->powered = true;
    self->sector_prn = 0xf;

    return 0;
}

int
fdc_load_disk (fdc_t *self, bool disk, char *filename)
{
    assert (self != NULL);
    assert (filename != NULL);

    FILE *fp = fopen (filename, "rb");
    if (fp == NULL)
    {
        log_error ("nsae: fdc: cannot open file: '%s'\n",
                filename);
        return 1;
    }

    size_t len = fread (self->data[disk], sizeof (uint8_t), FD_MAX_SIZE, fp);
    fclose (fp);

    switch (len)
    {
    case FD_SSDD_SIZE: self->disk_type[disk] = FD_SSDD; break;
    case FD_DSDD_SIZE: self->disk_type[disk] = FD_DSDD; break;
    default:
        log_error ("nsae: fdc: unknown disk type, %zuB\n",
                len);
        return 1;
    }

    self->disk_loaded[disk] = true;

    log_verbose ("nsae: fdc: loaded disk %d with '%s'\n", disk, filename);

    return 0;
}

int
fdc_save_disk (fdc_t *self, bool disk, char *filename)
{
    assert (self != NULL);
    assert (filename != NULL);

    if (!self->disk_loaded[disk])
    {
        log_error ("nsae: fdc: cannot save ejected disk\n");
        return 1;
    }

    FILE *fp = fopen (filename, "wb+");
    if (fp == NULL)
    {
        log_error ("nsae: fdc: cannot write to file: '%s'\n",
                filename);
        return 1;
    }

    size_t n = 0;
    switch (self->disk_type[disk])
    {
    case FD_SSDD: n = FD_SSDD_SIZE; break;
    case FD_DSDD: n = FD_DSDD_SIZE; break;
    }

    size_t len = fwrite (self->data[disk], sizeof (uint8_t), n, fp);
    fclose (fp);
    if (len != n)
    {
        log_error ("nsae: fdc: failed to write complete disk to file\n");
        return 1;
    }

    log_verbose ("nsae: fdc: saved disk %d into '%s'\n", disk, filename);

    return 0;
}

int
fdc_eject (fdc_t *self, bool disk)
{
    assert (self != NULL);

    if (!self->disk_loaded[disk])
    {
        log_error ("nsae: fdc: cannot eject from empty drive\n");
        return 1;
    }

    self->disk_loaded[disk] = false;

    log_verbose ("nsae: fdc: ejected disk %d\n", disk);

    return 0;
}

void
fdc_reset (fdc_t *self)
{
    assert (self != NULL);
}

void
fdc_load_drvctrl (fdc_t *self, uint8_t data)
{
    assert (self != NULL);
    //log_debug ("nsae: fdc: 0x%02x load drive control\n", data);

    /* bit 5 precompensation and step_direction */
    self->step_direction = (data >> 5) & 0x01;
    self->precompensation = self->step_direction;

    /* bit 6 side select */
    self->side = (data >> 6) & 0x01;

    /* bit 4 step pulse */
    if (!self->step_pulse && ((data >> 4) & 0x01))
    {
        fdc_step (self);
    }
    self->step_pulse = (data >> 4) & 0x01;

    /* bit 0-1 disk select */
    self->disk = (data >> 1) & 0x01;
}


static void 
fdc_secmark_low (fdc_t *self)
{
    //log_debug ("nsae: fdc: sector_mark high\n");

    fdc_set_read (self, false);     /* cleared on the 0-1 */
    fdc_set_write (self, false);    /* cleared on leading edge */

    self->sector_mark = true;
    self->sector_mark_hold = 10;
    //self->sector_mark_hold = 25;

    fdc_next_sector (self);

    /* data is not ready while we are over the sector mark */
    self->serial_data = false;
}

static void 
fdc_secmark_high (fdc_t *self)
{
    //log_debug ("nsae: fdc: sector_mark low\n");

    self->sector_mark = false;

    if (self->sector[self->disk] == 0xf)
    {
        self->sector_mark_hold = 10;
        self->serial_data = false;
    }
    else
    {
        self->sector_mark_hold = 40;
        self->serial_data = true;
    }
}


void
fdc_update (fdc_t *self)
{
    /* hi:lo 19850:150us */
    /* 25cycle loop at 4mhz = 6.25us */
    /* to high hold for 3300 iterations (normally ~20ms) */
    if ((self->sector_mark_hold == 0) && 
        (self->sector_mark))
    {
        fdc_secmark_high (self);
    }
    /* to low hold for 25 iterations (normally 150us) */
    else if ((self->sector_mark_hold == 0) && 
             (!self->sector_mark))
    {
        fdc_secmark_low (self);
    }
    else if ((self->powered && self->motor_enabled) &&
             !(self->read_mode || self->write_mode) &&
             (self->sector_mark_hold != 0))
    {
        self->sector_mark_hold--;
    }
}

void
fdc_set_read (fdc_t *self, bool state)
{
    assert (self != NULL);
    //log_debug ("nsae: fdc: %d set disk read\n", state);
    self->read_mode = state;
    self->index = 0;
    self->sync = 0;
}

void
fdc_set_write (fdc_t *self, bool state)
{
    assert (self != NULL);
    //log_debug ("nsae: fdc: %d set disk write\n", state);
    self->write_mode = state;
    self->index = 0;
    self->sync = 0;
}

void
fdc_start_motor (fdc_t *self)
{
    assert (self != NULL);
    log_debug ("nsae: fdc: start motor\n");
    self->motor_enabled = true;
}

void
fdc_stop_motor (fdc_t *self)
{
    assert (self != NULL);
    log_debug ("nsae: fdc: stop motor\n");
    self->motor_enabled = false;
}

void
fdc_step (fdc_t *self)
{
    assert (self != NULL);

    log_debug ("nsae: fdc: step\n");

    uint8_t *p_track = &self->track[self->disk];

    /* step in direction */
    if (self->step_direction == FD_STEP_IN)
    {
        /* protect from underflow */
        if (*p_track > 0) 
        {
            (*p_track)--;
        }
    }
    else if (self->step_direction == FD_STEP_OUT)
    {
        /* protect from overflow */
        if (*p_track < FD_TRACKS-1)
        {
            (*p_track)++;
        }
    }

    self->track_zero = (*p_track) == 0;
    //log_debug ("trackzero = %d\n", self->track_zero);
}


void
fdc_next_sector (fdc_t *self)
{
    self->sector_prn = self->sector[self->disk];
    if (self->sector_prn == 9)
    {
        self->sector_prn = 0x0f;
    }

    self->sector[self->disk]++;
    self->sector[self->disk] %= FD_SECTORS;
}


uint8_t
fdc_get_sector (fdc_t *self)
{
    assert (self != NULL);

    if (!self->powered)
    {
        return 0x0e;
    }

    return self->sector_prn;
}

uint8_t
fdc_read_sync1 (fdc_t *self)
{
    assert (self != NULL);
    self->sync = 1;
    return 0xfb;
}

uint8_t
fdc_read_sync2 (fdc_t *self)
{
    assert (self != NULL);

    uint8_t a = self->sector[self->disk];
    uint8_t b = ((uint16_t)self->track[self->disk] << 6) & 0x00f0;
    uint8_t c = b | a;

    return c;
}


uint32_t 
fdc_calc_disk_offset (uint8_t side, uint8_t track, uint8_t sector, uint16_t i)
{
    uint32_t abs_track = (side * FD_TRACKS) + track;
    uint32_t abs_sector = (abs_track * FD_SECTORS) + sector;
    uint32_t offset = (abs_sector * FD_BLKSIZE) + i;
    return offset;
}

static uint32_t
fdc_get_disk_offset (fdc_t *self)
{
    return fdc_calc_disk_offset (self->side,
                                 self->track[self->disk],
                                 self->sector[self->disk],
                                 0);
}

static uint8_t
fdc_calc_crc8 (uint8_t *buf, size_t n)
{
    uint8_t crc = 0;
    uint8_t oh = 0;

    for (; n > 0; n--, buf++)
    {
        crc ^= *buf;
        oh = (crc & 0x80) >> 7;
        crc &= ~0x80;
        crc <<= 1;
        crc |= oh;
        //printf ("crc8 calc: %3x: %02x %02x\n", n, *buf, crc);
    }

    return crc;
}

uint8_t
fdc_read (fdc_t *self)
{
    assert (self != NULL);
    
    if (!self->read_mode) return 0x00;

    if (self->sector[self->disk] >= FD_SECTORS)
    {
        log_error ("nsae: fdc: trying to read from bad sector -- %d\n",
                self->sector[self->disk]);
        return 0x00;
    }

    if (self->sync == 1)
    {
        log_debug ("nsae: fdc: read sync 2\n");
        self->sync = 2;
        return fdc_read_sync2 (self);
    }

    /* variables */
    uint32_t sec_base = fdc_get_disk_offset (self);
    uint32_t offset = sec_base + self->index;
    uint8_t data = self->data[self->disk][offset];

    /* crc */
    if (self->index >= FD_BLKSIZE)
    {
        log_debug ("nsae: fdc: read crc\n");
        fdc_secmark_low (self);

        /* calculate crc */
        return fdc_calc_crc8 (&self->data[self->disk][sec_base], FD_BLKSIZE);
    }

    /* data */
    log_debug ("nsae: fdc: read data 0x%02x from D%d Si%d T%2d Sec%2d I%2d (0x%08x)\n",
            data, 
            self->disk,
            self->side,
            self->track[self->disk],
            self->sector[self->disk],
            self->index,
            offset);

    self->index++;

    return data;
}

void
fdc_write (fdc_t *self, uint8_t data)
{
    assert (self != NULL);

    if (!self->write_mode) return;

    if (self->sector[self->disk] >= FD_SECTORS)
    {
        log_error ("nsae: fdc: trying to write to bad sector -- %d\n",
                self->sector[self->disk]);
        return;
    }

    /* preamble 33 bytes */
    if (self->preamble < 33)
    {
        log_debug ("nsae: fdc: write preamble %d %d\n", data, self->preamble);
        if (data != 0x00)
        {
            log_error ("nsae: fdc: bad preamble, data not 0x00\n");
        }
        self->preamble++;
        return;
    }

    /* sync1 byte */
    if (self->sync == 0)
    {
        log_debug ("nsae: fdc: write sync1 %d\n", data);
        if (data != 0xfb)
        {
            log_error ("nsae: fdc: bad sync1 byte\n");
        }
        self->sync++;
        return;
    }

    /* sync2 byte */
    if (self->sync == 1)
    {
        log_debug ("nsae: fdc: write sync2 %d\n", data);
        if (data != fdc_read_sync2 (self))
        {
            log_error ("nsae: fdc: bad sync2 byte\n");
        }
        self->sync++;
        return;
    }

    /* crc */
    if (self->index >= FD_BLKSIZE)
    {
        log_debug ("nsae: fdc: write crc %d\n", data);
        fdc_secmark_low (self);
        return;
    }

    /* data */
    uint32_t sec_base = fdc_get_disk_offset (self);
    uint32_t offset = sec_base + self->index;
    uint8_t *p_addr = &self->data[self->disk][offset];

    log_debug ("nsae: fdc: write data 0x%02x to D%d Si%d T%2d Sec%2d I%2d (%08x)\n",
            data, 
            self->disk,
            self->side,
            self->track[self->disk],
            self->sector[self->disk],
            self->index,
            offset);
    
    self->index++;

    *p_addr = data;
}

void
fdc_status (fdc_t *self)
{
    log_info ("power:     %1d\tmotor:   %1d\tdisk:    %1d\tside:  %1d\n",
            self->powered,
            self->motor_enabled,
            self->disk,
            self->side);
    log_info ("track0:    %1d\tspulse:  %1d\tsmark:   %1d\tsdata: %1d\n",
            self->track_zero,
            self->step_pulse,
            self->sector_mark,
            self->serial_data);
    log_info ("direction: %1d\tprecomp: %1d\tread:    %1d\twrite: %1d\n",
            self->step_direction,
            self->precompensation,
            self->read_mode,
            self->write_mode);
    log_info ("preamble: %2d\tsync:    %1d\tindex: %3d\tt: %2d\n",
            self->preamble,
            self->sync,
            self->index,
            self->sector_mark_hold);
    log_info ("disk 0: %4s\tt: %2d\ts: %2d\twp: %1d\t%s\n",
            (!self->disk_loaded[0] ? "    " :
                (self->disk_type[0] == FD_SSDD ? "SSDD" :
                 self->disk_type[0] == FD_DSDD ? "DSDD" : "????")),
            self->track[0],
            self->sector[0],
            self->hard_ro,
            (!self->disk_loaded[0] ? "none" : self->filename[0]));
    log_info ("disk 1: %4s\tt: %2d\ts: %2d\twp: %1d\t%s\n",
            (!self->disk_loaded[1] ? "    " :
                (self->disk_type[1] == FD_SSDD ? "SSDD" :
                 self->disk_type[1] == FD_DSDD ? "DSDD" : "????")),
            self->track[1],
            self->sector[1],
            self->hard_ro,
            (!self->disk_loaded[1] ? "none" : self->filename[1]));
}


/* end of file */
