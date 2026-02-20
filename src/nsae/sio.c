
#define LOG_CATEGORY LC_SIO
#include "sio.h"

#include "nslog.h"

/*
 * io reset:
 *   1. interupt mask cleared,
 *   2. baud rate register = 0
 *   3. ready for reprogramming
 *
 * out xAH program interupt mask
 * A 1 in any position allows that states to generate a maskable interupt
 *   b0 Tx Empty
 *   b1 Tx Ready
 *   b2 Rx Ready
 *   b3 Sync Detect
 *   b4-b7 unused
 *
 * out x1H programming control bytes
 * in  x1H read status byte
 * in/out x0H read/write USART data
 * out x8H program baud rate register.
 *
 *
 * async baud rate
 * baud     x16     x64
 * 19200    7F      --
 *  9600    7E      --
 *  4800    7C      7F
 *  2400    78      7E
 *  1200    70      7C
 *   600    60      78
 *   300    40      70
 *   200    20      68
 *   150    00      60
 *   110    --      54
 *    75    --      40
 *    50    --      20
 *    45    --      16
 *
 * sync baud rate
 * baud hex
 * 51000    7A
 * 38400    78
 * 19200    70
 *  9600    60
 *  4800    40
 *  2400    00
 */

/* uPD8251 documentation */
/* mode format */
#define MODE_BS         0x00    /* (sync) select mode */
#define MODE_BA1        0x01    /* (async) baud rate 1x */
#define MODE_BA16       0x02    /* (async) baud rate 16x */
#define MODE_BA64       0x03    /* (async) baud rate 64x */
#define MODE_MASK_BAUD  0x03

#define MODE_L5         0x00    /* 5-bit characters */
#define MODE_L6         0x04    /* 6-bit character */
#define MODE_L7         0x08    /* 7-bit character */
#define MODE_L8         0x0c    /* 8-bit character */
#define MODE_MASK_LEN   0x0c

#define MODE_PEN        0x10    /* parity enable */
#define MODE_EP         0x20    /* even parity */

#define MODE_A_S1       0x40    /* (async) 1   stop bit */
#define MODE_A_S1_5     0x80    /* (async) 1.5 stop bits */
#define MODE_A_S2       0xc0    /* (async) 2   stop bits */
#define MODE_MASK_STOP  0xc0

#define MODE_S_ESO      0x40    /* (sync) external sync detect */
#define MODE_S_SCS      0x80    /* (sync) single character */

/* command format */
#define CMMD_TXEN       0x01    /* transmit enable */
#define CMMD_DTR        0x02    /* data terminal ready */
#define CMMD_RXE        0x04    /* recieve enable */
#define CMMD_SBRK       0x08    /* send break character */
#define CMMD_ER         0x10    /* error reset */
#define CMMD_RTS        0x20    /* request to send */
#define CMMD_IR         0x40    /* internal reset */
#define CMMD_EH         0x80    /* (sync) enter hunt mode */

#define STAT_TXRDY      0x01    /* (pin) transmitter ready */
#define STAT_RXRDY      0x02    /* (pin) reciever ready */
#define STAT_TXE        0x04    /* (pin) transmitter empty */
#define STAT_PE         0x08    /* parity error */
#define STAT_OE         0x10    /* overrun error */
#define STAT_FE         0x00    /* (async) framing error */
#define STAT_SYNDET_BD  0x40    /* (pin) sync detect/async break detect */
#define STAT_DSR        0x80    /* (pin) data set ready */

#define DATA_BLK_SIZE 0x200     /* block size of data */

int
sio_init (sio_t *self, const char *host_device)
{
    (void)host_device; /* silence unused warning */

    *self = (sio_t){
        .interupt_mask = 0x00,
        .status = 0x00,
        .baud_code = 0x00,
        .mode = 0x00,
        .command = 0x00,
        .context = EXPECT_MODE,
        .data_index = 0,
    };

    return 0;
}

int
sio_destroy (sio_t *self)
{
    return 0;
}

uint8_t
sio_board_id (sio_t *self)
{
    /* in a,(00Xh) */
    const uint8_t SIO_BOARD_ID = 0xf7;
    log_debug ("nsae: sio: prompt board type - %02X\n", SIO_BOARD_ID);
    return SIO_BOARD_ID;
}

void
sio_reset (sio_t *self)
{
    /* When the I/O (Reset bit (I/O address F0H, bit 4) is set on, then off */
    log_debug ("nsae: sio: io-board reset\n");
    self->interupt_mask = 0x00;
    self->baud_code = 0x00;
    self->context = EXPECT_MODE;
    self->data_index = 0;
}

uint8_t
sio_recieve_data (sio_t *self)
{
    uint8_t data = 0x00;
    /* in a,(0X0h) */
    //log_debug ("nsae: sio: recieve data #-03u - %02X\n", 
    //        self->data_index, data);
    return data;
}

void
sio_send_data (sio_t *self, uint8_t data)
{
    /* out (0X0h),a*/
    //log_debug ("nsae: sio: send data #-03u - %02X\n", 
    //        self->data_index, data);
    return;
}

uint8_t
sio_get_status (sio_t *self)
{
    /* in a,(0X1h) */
    //log_debug ("nsae: sio: read status - %02X\n", self->status);
    return self->status;
}

void
sio_program (sio_t *self, uint8_t data)
{
    /* out (0X1h),a */
    int sync_mode = 0;

    switch (self->context)
    {
    case EXPECT_MODE:
        log_debug ("nsae: sio: program mode - %02x\n", data);
        self->mode = data;
        sync_mode = (self->mode & MODE_MASK_BAUD) == MODE_BS;
        /* logic */

        self->context = ((sync_mode) ? EXPECT_SYNC1 : EXPECT_DATA);
        break;

    case EXPECT_SYNC1:
        log_debug ("nsae: sio: output sync 1 - %02x\n", data);
        self->context = EXPECT_SYNC2;
        break;

    case EXPECT_SYNC2:
        log_debug ("nsae: sio: output sync 2 - %02x\n", data);
        self->context = EXPECT_SYNC2;
        break;

    case EXPECT_COMMAND:
        log_debug ("nsae: sio: output command - %02x\n", data);
        self->command = data;
        self->data_index = 0;
        self->context = EXPECT_DATA;
        break;

    case EXPECT_DATA:
    default:
        log_error ("nsae: sio: unexpected command at this time\n");
        break;
    }
}

void
sio_set_baud (sio_t *self, uint8_t data)
{
    /* out (0X8h),a ;or 0X9h set baud rate */
    //log_debug ("nsae: sio: set baud rate - %02x\n", data);
    self->baud_code = data;
}

void
sio_set_interupt_mask (sio_t *self, uint8_t data)
{
    /* out (0XAh),a ;or 0XBh set interupt mask */
    //log_debug ("nsae: sio: set interupt mask - %02x\n", data);
    self->interupt_mask = data;
}

/* end of file */
