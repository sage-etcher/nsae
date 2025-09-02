
#include "fdc.h"

#include "log.h"
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
fdc_start_motor (fdc_t *self)
{
    assert (self != NULL);
    log_verbose ("nsae: fdc: start motor\n");
    self->motor_enabled = true;
}

void
fdc_stop_motor (fdc_t *self)
{
    assert (self != NULL);
    log_verbose ("nsae: fdc: stop motor\n");
    self->motor_enabled = false;
}

void
fdc_step (fdc_t *self)
{
    assert (self != NULL);

    log_verbose ("nsae: fdc: step\n");

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


uint8_t
fdc_get_sector (fdc_t *self)
{
    assert (self != NULL);
    uint8_t sec = self->sector[self->disk];

    //self->preamble = 0;
    //self->sync = 0;
    //self->index = 0;
    //self->read_mode = false;
    //self->write_mode = false;

    return sec;
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
    uint8_t b = ((uint16_t)self->track[self->disk] << 4) & 0x00f0;
    uint8_t c = b | a;

    return c;
}

static uint32_t
fdc_get_disk_offset (fdc_t *self)
{
    uint32_t offset = self->side;
    offset *= FD_TRACKS;
    offset += self->track[self->disk];
    offset <<= 4;
    offset += self->sector[self->disk];
    offset *= FD_BLKSIZE;

    return offset;
}

uint8_t
fdc_read (fdc_t *self)
{
    assert (self != NULL);

    if (!self->read_mode) return 0x00;

    if (self->sync == 1)
    {
        log_fdc ("nsae: fdc: read sync 2\n");
        self->sync = 2;
        return fdc_read_sync2 (self);
    }

    if (self->index >= FD_BLKSIZE)
    {
        log_fdc ("nsae: fdc: read crc\n");
        /* crc */
        self->sync = 0;
        self->index = 0;
        self->read_mode = false;
        self->sector[self->disk]++;
        self->sector[self->disk] %= FD_SECTORS;
        self->sector_mark = true;
        self->sector_mark_hold = 40;
        self->serial_data = true;

        /* calculate crc */
        return 0x00;
    }

    uint32_t sec_base = fdc_get_disk_offset (self);
    uint32_t offset = sec_base + self->index;
    uint8_t data = self->data[self->disk][offset];

    log_fdc ("nsae: fdc: read data 0x%02x from D%d Si%d T%2d Sec%2d I%2d (0x%08x)\n",
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

    /* preamble 33 bytes */
    if (self->preamble < 33)
    {
        log_fdc ("nsae: fdc: write preamble %d %d\n", data, self->preamble);
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
        log_fdc ("nsae: fdc: write sync1 %d\n", data);
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
        log_fdc ("nsae: fdc: write sync2 %d\n", data);
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
        log_fdc ("nsae: fdc: write crc %d\n", data);
        /* crc */
        self->preamble = 0;
        self->sync = 0;
        self->index = 0;
        self->write_mode = false;
        self->sector[self->disk]++;
        self->sector[self->disk] %= FD_SECTORS;
        self->sector_mark = true;
        self->sector_mark_hold = 40;
        self->serial_data = true;
        return;
    }

    /* data */
    uint32_t sec_base = fdc_get_disk_offset (self);
    uint32_t offset = sec_base + self->index;
    uint8_t *p_addr = &self->data[self->disk][offset];

    log_fdc ("nsae: fdc: write data 0x%02x to D%d Si%d T%2d Sec%2d I%2d (%08x)\n",
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

/* end of file */
