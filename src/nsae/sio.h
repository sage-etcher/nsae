
#ifndef NSAE_SIO_H
#define NSAE_SIO_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EXPECT_MODE,
    EXPECT_SYNC1,
    EXPECT_SYNC2,
    EXPECT_COMMAND,
    EXPECT_DATA,
} sio_ctx_t;

typedef struct {
    uint8_t interupt_mask;      /* status mask controls adv interupt flag */
    uint8_t status;             /* status flags */
    uint8_t baud_code;          /* configure once baud */
    uint8_t mode;               /* configure once mode */ 
    uint8_t command;            /* command */
    sio_ctx_t context;          /* stage of operation */
    uint_least16_t data_index;  /* how many peices of data have we sent */
} sio_t;

int sio_init (sio_t *self, const char *host_device);
int sio_destroy (sio_t *self);

uint8_t sio_board_id (sio_t *self);
void sio_reset (sio_t *self);
uint8_t sio_recieve_data (sio_t *self);
void sio_send_data (sio_t *self, uint8_t data);
uint8_t sio_get_status (sio_t *self);
void sio_program (sio_t *self, uint8_t data);
void sio_set_baud (sio_t *self, uint8_t data);
void sio_set_interupt_mask (sio_t *self, uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_SIO_H */
/* end of file */
