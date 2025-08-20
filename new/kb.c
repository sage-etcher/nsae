
#include "kb.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
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

    /* set overflow */
    if (self->buf_cnt >= KB_BUF_MAX)
    {
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

    return self->buf[0] & 0x0f;
}

uint8_t
kb_get_msb (kb_t *self)
{
    assert (self != NULL);

    return (kb_pop (self) & 0xf0) >> 4;
}


/* end of file */
