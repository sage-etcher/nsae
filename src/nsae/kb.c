
#include "kb.h"

#include "log.h"
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

uint8_t
kb_decode_key (kb_t *self, uint8_t host_key)
{
    /* TODO: handle modifier conversion */
    return host_key;
}

void
kb_push (kb_t *self, uint8_t key)
{
    assert (self != NULL);

    log_kb ("nsae: kb: push %02x '%c'\n", key, key);

    /* set overflow */
    if (self->buf_cnt >= KB_BUF_MAX)
    {
        log_kb ("nsae: kb: overflow\n");
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

    log_kb ("nsae: kb: pop  %02x '%c'\n", key, key);

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

    log_kb ("nsae: kb: reading lsb 0x%02x\n", lsb);

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

    log_kb ("nsae: kb: reading msb 0x%02x\n", msb);

    return msb;
}


/* end of file */
