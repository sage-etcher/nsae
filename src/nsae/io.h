
#ifndef NSAE_IO_H
#define NSAE_IO_H
#ifdef __cplusplus
extern "c" {
#endif

#include "sio.h"
#include "pio.h"
#include "hdc.h"

#include <stdbool.h>
#include <stdint.h>

enum {
    IO_NONE = 0xff, /* no board or board w/o ID (video board) */
    IO_FPB  = 0x7f, /* (unreleased) floating point board */
    IO_SIO  = 0xf7, /* serial board */
    IO_PIO  = 0xdb, /* parallel board */
    IO_HDC  = 0xbe, /* harddisk controller board */
};

#define IO_SLOT_CNT 6
typedef struct {
    uint8_t slot[IO_SLOT_CNT];
    union {
        sio_t sio;
        pio_t pio;
        hdc_t hdc;
    } m[IO_SLOT_CNT];
} io_t;

int io_init (io_t *self);

int io_check_interupt (io_t *self);

int io_attach_card (io_t *self, uint8_t slot, uint8_t card, const void *data);
int io_dettach_card (io_t *self, uint8_t slot);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_IO_H */
/* end of file */
