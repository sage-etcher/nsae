
#define LOG_CATEGORY LC_IO
#include "io.h"

#include "hdc.h"
#include "nslog.h"
#include "pio.h"
#include "sio.h"

#include <assert.h>
#include <string.h>

int
io_init (io_t *self)
{
    /* initialize to IO_SLOT to avoid errors from dettach */
    memset (self->slot, IO_NONE, sizeof (self->slot));

    io_attach_card (self, 1, IO_SIO, "/dev/ttyS0");
    io_attach_card (self, 2, IO_NONE, NULL);
    io_attach_card (self, 3, IO_NONE, NULL);
    io_attach_card (self, 4, IO_NONE, NULL);
    io_attach_card (self, 5, IO_NONE, NULL);
    io_attach_card (self, 6, IO_NONE, NULL);

    return 0;
}

int
io_check_interupt (io_t *self)
{
    int i = 0;
    sio_t *sio = &self->m[i].sio;
    pio_t *pio = &self->m[i].pio;
    hdc_t *hdc = &self->m[i].hdc;

    (void)pio;
    (void)hdc;

    for (i = 0; i < IO_SLOT_CNT; i++) /* unroll me, it's easy */
    {
        switch (self->slot[i])
        {
        case IO_SIO:
            if (sio->status & sio->interupt_mask)
            {
                return 1;
            }
            break;

        case IO_PIO:  break; /* unimplimented */
        case IO_HDC:  break; /* unimplimented */
        case IO_FPB:  break; /* unimplimented */
        case IO_NONE: break;

        default:
            log_error ("nsae: io: error: unknown board type in slot #%d\n", 
                    i+1);
            break;
        }
    }

    return 0;
}

int
io_attach_card (io_t *self, uint8_t slot, uint8_t card, const void *data)
{
    uint8_t slot_index = 0;
    uint8_t card_type = 0;

    assert (self != NULL);
    assert (slot > 0);
    assert (slot <= IO_SLOT_CNT);

    slot_index = slot - 1;

    (void)io_dettach_card (self, slot);

    card_type = card;
    self->slot[slot_index] = card;

    switch (card)
    {
    case IO_SIO: return sio_init (&self->m[slot_index].sio, data);
    case IO_NONE: return 0;

    case IO_PIO:
    case IO_HDC:
    case IO_FPB:
        log_error ("nsae: io: attaching unimpliement card type - %02x\n",
                card_type);
        return 0;

    default:
        /* unknown slot type */
        log_error ("nsae: io: attaching card of unknown type - %02x\n",
                card_type);
        self->slot[slot_index] = IO_NONE;
        return 1;
    }
}

int
io_dettach_card (io_t *self, uint8_t slot)
{
    uint8_t slot_index = 0;
    uint8_t card_type = 0;

    assert (self != NULL);
    assert (slot > 0);
    assert (slot <= IO_SLOT_CNT);

    slot_index = slot - 1;

    card_type = self->slot[slot_index];
    self->slot[slot_index] = IO_NONE;

    switch (card_type)
    {
    case IO_SIO: return sio_destroy (&self->m[slot_index].sio);
    case IO_NONE: return 0;

    case IO_PIO:
    case IO_HDC:
    case IO_FPB:
        /* unknown slot type */
        log_error ("nsae: io: dettaching unimplimented card - %02x\n",
                card_type);
        return 0;

    default:
        /* unknown slot type */
        log_error ("nsae: io: dettaching card of unknown type - %02x\n",
                card_type);
        return 1;
    }
}


/* end of file */
