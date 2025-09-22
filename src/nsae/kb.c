
#define LOG_CATEGORY LC_KB
#include "kb.h"

#include "nslog.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


int
kb_init (kb_t *self)
{
    assert (self != NULL);

    memset (self, 0, sizeof (*self));

    return 0;
}

void
kb_reset (kb_t *self)
{
    self->buf_cnt     = 0;
    self->data_flag   = 0;
    self->cursor_lock = 0;
    self->caps_lock   = 0;
    self->autorepeat  = 0;
    self->overflow    = 0;
}

void
kb_push (kb_t *self, uint8_t key)
{
    assert (self != NULL);

    log_debug ("nsae: kb: push %02x '%c'\n", key, key);

    /* set overflow */
    if (self->buf_cnt >= KB_BUF_MAX)
    {
        log_debug ("nsae: kb: overflow\n");
        self->overflow = true;
        return;
    }

    /* append key */
    self->buf[self->buf_cnt++] = key;

    /* set data flag */
    self->data_flag = true;
}

uint8_t
kb_pop (kb_t *self)
{
    assert (self != NULL);

    /* save bottom value */
    uint8_t key = self->buf[0];

    log_debug ("nsae: kb: pop  %02x '%c'\n", key, key);

    /* shift buf left 1 */
    if (self->buf_cnt != 0)
    {
        self->buf_cnt--;
    }
    memcpy (self->buf, self->buf+1, self->buf_cnt);

    /* set keybaord data flag */
    self->data_flag = self->buf_cnt != 0;

    return key;
}

uint8_t
kb_get_lsb (kb_t *self)
{
    assert (self != NULL);

    if (!self->data_flag)
    {
        log_warning ("nsae: kb: reading lsb from empty buffer\n");
    }

    uint8_t lsb = self->buf[0] & 0x0f;

    log_debug ("nsae: kb: reading lsb 0x%02x\n", lsb);

    return lsb;
}

uint8_t
kb_get_msb (kb_t *self)
{
    assert (self != NULL);

    if (!self->data_flag)
    {
        log_warning ("nsae: kb: reading MSB from empty buffer\n");
    }

    uint8_t msb = (kb_pop (self) & 0xf0) >> 4;

    log_debug ("nsae: kb: reading msb 0x%02x\n", msb);

    return msb;
}

void
kb_status (kb_t *self)
{
    log_info ("overflow: %1d\tdata_flag: %1d\treset: %1d\n",
            self->overflow,
            self->data_flag,
            self->reset);
    log_info ("cursor_lock: %1d\tcaps_lock: %1d\tautorepeat: %1d\n", 
            self->cursor_lock,
            self->caps_lock,
            self->autorepeat);
    log_info ("buf[]: %02x %02x %02x %02x %02x %02x %02x\n", 
            self->buf[0], self->buf[1], self->buf[2],
            self->buf[3], self->buf[4], self->buf[5],
            self->buf[6]);
    log_info ("buf_cnt: %1d\n", self->buf_cnt);
}


/* end of file */
