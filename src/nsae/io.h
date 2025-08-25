
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
    IO_NONE,
    IO_SIO,
    IO_PIO,
    IO_HDC,
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

int io_attach_card (io_t *self, uint8_t slot, uint8_t card);
int io_dettach_card (io_t *self, uint8_t slot);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_IO_H */
/* end of file */
