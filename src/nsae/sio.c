
#define LOG_CATEGORY LC_SIO
#include "sio.h"

#include "nsae_config.h"
#include "nslog.h"

#include <sc_time.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ARRLEN(a) (sizeof (a) / sizeof (*(a)))

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

#define MODE_A_SINVALID 0x00    /* (async) invalid combination */
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

static int configure_host_mode (sio_t *self);


int
sio_init (sio_t *self, const char *host_device)
{
#if ENABLE_SERIAL_PORT_EMULATION
    int serial_port = 0;
    struct termios tty = { 0 };
#endif

    *self = (sio_t){
        .interupt_mask = 0x00,
        .status = 0x00,
        .baud_code = 0x00,
        .mode = 0x00,
        .command = 0x00,
        .context = EXPECT_MODE,
        .data_index = 0,
#if ENABLE_SERIAL_PORT_EMULATION
        .dev_path = NULL,
        .serial_port = 0,
        .tty =  { 0 },
#endif
    };

#if ENABLE_SERIAL_PORT_EMULATION
    if (host_device == NULL) 
    {
        return 0;
    }

    serial_port = open (host_device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_port < 0)
    {
        log_error ("nsae: sio: failed to open: %s: %s\n",
                host_device, strerror (errno));
        return 1;
    }

    if (!isatty (serial_port))
    {
        log_error ("nsae: sio: dev not a serial device - %s\n", host_device);
        close (serial_port);
        return 1;
    }

    if (tcgetattr (serial_port, &tty) != 0)
    {
        log_error ("nsae: sio: tcgetattr: %d - %s\n",
                errno, strerror (errno));
        close (serial_port);
        return 1;
    }

    self->dev_path = host_device;
    self->serial_port = serial_port;
    self->tty = tty;

    configure_host_mode (self);

#else

    (void)host_device; /* silence unused */
#endif

    return 0;
}

int
sio_destroy (sio_t *self)
{
    if (self == NULL) 
    {
        return 0;
    }

#if ENABLE_SERIAL_PORT_EMULATION
    close (self->serial_port);
#endif

    memset (self, 0, sizeof (*self));

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

#define FLAG_DISABLE(v,f)   (v) &= ~(f)
#define FLAG_ENABLE(v,f)    (v) |= (f)

static uint16_t 
decode_baud_rate (uint8_t multiplier, uint8_t code)
{
    /* setup baud rate */
    switch (multiplier)
    {
    case MODE_BA16:
        switch (code)
        {
        case 0x7f: return B19200;
        case 0x7e: return B9600;
        case 0x7c: return B4800;
        case 0x78: return B2400;
        case 0x70: return B1200;
        case 0x60: return B600;
        case 0x40: return B300;
        case 0x20: return B200;
        case 0x00: return B150;
        }
        break;

    case MODE_BA64:
        switch (code)
        {
        case 0x7f: return B4800;
        case 0x7e: return B2400;
        case 0x7c: return B1200;
        case 0x78: return B600;
        case 0x70: return B300;
        case 0x68: return B200;
        case 0x60: return B150;
        case 0x54: return B110;
        case 0x40: return B75;
        case 0x20: return B50;

        case 0x16: 
            log_warning ("nsae: sio: using non-POSIX baud rate - 45\n");
            return 45; 
        }
        break;

    case MODE_BA1:
        log_error ("nsae: sio: unsupported feature baud multiplier 1x\n");
        break;

    case MODE_BS:
        log_error ("nsae: sio: unsupported feature syncronous serial\n");
        break;
    }

    return 0;
}

static int
configure_host_mode (sio_t *self)
{
#if ENABLE_SERIAL_PORT_EMULATION
    uint16_t baud_rate = 0;
    struct termios tty = self->tty;

    /* get baud rate */
    baud_rate = decode_baud_rate (self->mode & MODE_MASK_BAUD, self->baud_code);
    if (baud_rate == 0)
    {
        log_error ("nsae: sio: baud rate cannot be 0\n");
        return 1;
    }

    /* configure serial port */
    tty.c_cflag = 0;
    tty.c_iflag = 0;
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    /* enable/disable parity */
    if (self->mode & MODE_PEN)
    {
        FLAG_ENABLE (tty.c_cflag, PARENB);
    }

    /* even/odd parity */
    if (~self->mode & MODE_EP)
    {
        FLAG_ENABLE (tty.c_cflag, PARODD);
    }

    /* stop bit configuration */
    switch (self->mode & MODE_MASK_STOP)
    {
    case MODE_A_S1: FLAG_DISABLE (tty.c_cflag, CSTOPB); break; /* do nothing */
    case MODE_A_S2: FLAG_ENABLE  (tty.c_cflag, CSTOPB); break;

    case MODE_A_S1_5:
        log_error ("nsae: sio: unsupported feature serial 1.5-bit stop\n");
        return 1;

    case MODE_A_SINVALID:
        log_error ("nsae: sio: invalid stop bit selected\n");
        return 1;
    }

    /* nubmer of bits per byte */
    FLAG_DISABLE (tty.c_cflag, CSIZE);
    switch (self->mode & MODE_MASK_LEN)
    {
    case MODE_L5: FLAG_ENABLE (tty.c_cflag, CS5); break;
    case MODE_L6: FLAG_ENABLE (tty.c_cflag, CS6); break;
    case MODE_L7: FLAG_ENABLE (tty.c_cflag, CS7); break;
    case MODE_L8: FLAG_ENABLE (tty.c_cflag, CS8); break;
    }

    /* other unix config */
#if defined CRTSCTS
    FLAG_ENABLE (tty.c_cflag, CRTSCTS); /* enable hardware control flow */
#endif
    /* enable read
     * enable parity+framing errors
     * enable break character */
    FLAG_ENABLE (tty.c_cflag, CREAD | INPCK | PARMRK | BRKINT);

    /* hang max 1s until byte is ready */
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 10;

    /* set baud rate */
    cfsetispeed (&tty, baud_rate);
    cfsetospeed (&tty, baud_rate);

    /* apply changes */
    self->tty = tty;
    tcflush (self->serial_port, TCIFLUSH);
    tcsetattr (self->serial_port, TCSANOW, &self->tty);

#else

    (void)self;
#endif

    return 0;
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

    configure_host_mode (self);
}

uint8_t
sio_recieve_data (sio_t *self)
{
    uint8_t data = 0x00;
    /* in a,(0X0h) */
    //log_debug ("nsae: sio: recieve data #-03u - %02X\n", 
    //        self->data_index, data);

#if ENABLE_SERIAL_PORT_EMULATION
    int n = read (self->serial_port, &data, sizeof (uint8_t)); 
    if (n < 0) 
    {
        log_error ("nsae: sio: failed to read byte from serial port\n");
        return 0x00;
    }

    if (data == 0xff)
    {
        do {
            n = read (self->serial_port, &data, sizeof (uint8_t)); 
            sc_time_sleep (10);
        } while (n == 0);

        if (n < 0)
        {
            log_error ("nsae: sio: failed to read byte from serial port\n");
            return 0x00;
        }

        switch (data)
        {
        case 0x00:
            do {
                n = read (self->serial_port, &data, sizeof (uint8_t)); 
                sc_time_sleep (10);
            } while (n == 0);

            if (n < 0) 
            {
                log_error ("nsae: sio: failed to read byte from serial port\n");
                return 0x00;
            }

            self->status |= STAT_FE | STAT_PE;
            break;

        case 0xff:
            break;

        default:
            log_warning ("nsae: sio: unknown escape sequence: 0xff 0x%02x\n", 
                    data);
            break;
        }
    }
#endif

    return data;
}

void
sio_send_data (sio_t *self, uint8_t data)
{
    /* out (0X0h),a*/
    //log_debug ("nsae: sio: send data #-03u - %02X\n", 
    //        self->data_index, data);
#if ENABLE_SERIAL_PORT_EMULATION
    int n = write (self->serial_port, &data, sizeof (uint8_t)); 
    if (n < 0) 
    {
        log_error ("nsae: sio: failed to write byte to serial port\n");
    }
#endif

    return;
}

uint8_t
sio_get_status (sio_t *self)
{
    int rc = 0;
    int modem_status = 0;

    /* in a,(0X1h) */
    //log_debug ("nsae: sio: read status - %02X\n", self->status);

    /* get modem bits */
    rc = ioctl (self->serial_port, TIOCMGET, &modem_status);
    if (rc == -1)
    {
        log_error ("nsae: sio: failed to read status - %s\n", 
                strerror (errno));
        return STAT_RXRDY | STAT_TXRDY | STAT_DSR;
    }

    /* translate to advantage */
    self->status = 0x00;

    if (modem_status & TIOCM_LE)
    {
        self->status |= STAT_DSR;
    }

    if (modem_status & TIOCM_ST)
    {
        self->status |= STAT_TXRDY;
    }

    if (modem_status & TIOCM_SR)
    {
        self->status |= STAT_RXRDY;
    }

    /* unsure how to handle STAT_TXE or SYNDET_BD on unix api */
    /* TODO: overrun error */

    /* parity and framing errors are handled on sio_reieve_data */

    return self->status;
}
        
static int 
decode_command (sio_t *self)
{
    const struct { int src; int dst; } MAP[] = {
        { .src = CMMD_TXEN, .dst = TIOCM_ST },
        { .src = CMMD_DTR,  .dst = TIOCM_DTR },
        { .src = CMMD_RXE,  .dst = TIOCM_SR },
        { .src = CMMD_RTS,  .dst = TIOCM_RTS },
    };
    const size_t MAP_LEN = ARRLEN (MAP);
    size_t i = 0;
    int modem_status = 0;
    int bit_mask = 0;
    int action = 0;


    ioctl (self->serial_port, TIOCMGET, &modem_status);

    for (i = 0; i < MAP_LEN; i++)
    {
        bit_mask = MAP[i].dst;
        action = ((self->command & MAP[i].src) ? TIOCMBIS : TIOCMBIC);
        ioctl (self->serial_port, action, &bit_mask);
    }

    if (self->command & CMMD_SBRK)
    {
        /* send break */
        tcsendbreak (self->serial_port, 0);
    }

    if (self->command & CMMD_ER)
    {
        /* clears all errors on the serial device */
        self->status &= ~(STAT_FE | STAT_OE | STAT_PE);
    }

    if (self->command & CMMD_IR)
    {
        /* i dont remember how many times but iirc toggling this 3 times or
         * smthn resets the serial card specifically */
    }

    if (self->command & CMMD_EH)
    {
        /* enter huntmode */
        log_error ("nsae: sio: hunt mode is not supported by the emulator\n");
        return 1;
    }


    return 0;
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

        configure_host_mode (self);

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
        decode_command (self);
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
    configure_host_mode (self);
}

void
sio_set_interupt_mask (sio_t *self, uint8_t data)
{
    /* out (0XAh),a ;or 0XBh set interupt mask */
    //log_debug ("nsae: sio: set interupt mask - %02x\n", data);
    self->interupt_mask = data;
}

/* end of file */
