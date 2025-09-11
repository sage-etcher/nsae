
#ifndef NSAECMD_H
#define NSAECMD_H

#include <stdint.h>

/*  virtual "union" sketch
 *   0      packet_size
 *   1      "
 *   2      "
 *   3      "
 *   4      cmd
 *   5
 *   6
 *   7
 *   8      span32  data    data    fd_num      slot    state   keycode
 *   9      "               port    fd_side     page
 *  10      "               span16  fd_track
 *  11      "               "       fd_sector
 *  12      addr32  addr32  addr16  buflen
 *  13      "               "       "
 *  14      "                       "
 *  15      "                       "
 *  16+     abstract buffer data
 */

typedef struct {
    uint8_t cmd;
    uint8_t x[3];  /* unused padding */
    union {
        uint32_t u32;
        uint16_t u16[2];
        uint8_t u8[4];
    } a;
    union {
        uint32_t u32;
        uint16_t u16[2];
        uint8_t u8[4];
    } b;
    uint8_t buf[];
} nsae_packet_t;

/* virtual union */
#define v_span32    a.u32       /* 0 - 3    */
#define v_data      a.u8[0]     /* 0        */
#define v_fd_num    a.u8[0]     /* 0        */
#define v_slot      a.u8[0]     /* 0        */
#define v_state     a.u8[0]     /* 0        */
#define v_keycode   a.u8[0]     /* 0        */

#define v_port      a.u8[1]     /* 1        */
#define v_fd_side   a.u8[1]     /* 1        */
#define v_page      a.u8[1]     /* 1        */

#define v_span16    a.u16[1]    /* 2 - 3    */
#define v_fd_track  a.u8[2]     /* 2        */

#define v_fd_sector a.u8[3]     /* 3        */

#define v_buflen    b.u32       /* 4 - 7    */
#define v_addr32    b.u32       /* 4 - 7    */
#define v_addr16    b.u16[0]    /* 4 - 5    */

/* cmd types */
typedef enum {

    /* emulator */
    NSAE_CMD_NOP,
    NSAE_CMD_EXIT,
    NSAE_CMD_RESTART,
    NSAE_CMD_PAUSE,
    NSAE_CMD_CONTINUE,
    NSAE_CMD_BRKPNT_SET,    /* addr16                               */
    NSAE_CMD_BRKPNT_REMOVE, /* addr16                               */
    NSAE_CMD_BRKPNT_LIST,
    NSAE_CMD_STEP,
    NSAE_CMD_RUN,
    NSAE_CMD_STATUS,

    /* logging */
    NSAE_CMD_LOG_CPU,       /* state                                */
    NSAE_CMD_LOG_MMU,       /* state                                */
    NSAE_CMD_LOG_RAM,       /* state                                */
    NSAE_CMD_LOG_FDC,       /* state                                */
    NSAE_CMD_LOG_CRT,       /* state                                */
    NSAE_CMD_LOG_KB,        /* state                                */
    NSAE_CMD_LOG_MOBO,      /* state                                */
    NSAE_CMD_LOG_TERSE,
    NSAE_CMD_LOG_VERBOSE,
    NSAE_CMD_LOG_DEBUG,
    NSAE_CMD_LOG_OUTPUT,    /* buflen buf                           */

    /* floppydisk */
    NSAE_CMD_FD_EJECT,      /* fd_num                               */
    NSAE_CMD_FD_LOAD,       /* fd_num buflen buf                    */
    NSAE_CMD_FD_SAVE,       /* fd_num buflen buf                    */
    NSAE_CMD_FD_BLK_READ,   /* fd_num fd_side fd_track fd_sector    */
    NSAE_CMD_FD_STATUS,

    /* harddisk */
    NSAE_CMD_HD_EJECT,
    NSAE_CMD_HD_LOAD,       /* buflen buf                           */
    NSAE_CMD_HD_SAVE,       /* buflen buf                           */
    NSAE_CMD_HD_STATUS,

    /* keyboard */
    NSAE_CMD_KB_PUSH,       /* keycode                              */
    NSAE_CMD_KB_POP,
    NSAE_CMD_KB_OVERFLOW,   /* state                                */
    NSAE_CMD_KB_CAPS,       /* state                                */
    NSAE_CMD_KB_CURSOR,     /* state                                */
    NSAE_CMD_KB_DATA,       /* state                                */
    NSAE_CMD_KB_INTERUPT,   /* state                                */
    NSAE_CMD_KB_STATUS,

    /* io boards */
    NSAE_CMD_IO_STATUS,

    /* display */
    NSAE_CMD_CRT_STATUS,

    /* advantage */
    NSAE_CMD_ADV_OUT,       /* port data8                           */
    NSAE_CMD_ADV_IN,        /* port                                 */
    NSAE_CMD_ADV_STATUS,

    /* cpu */
    NSAE_CMD_CPU_STATUS,

    /* ram */
    NSAE_CMD_RAM_READ,      /* addr32 span32                        */
    NSAE_CMD_RAM_WRITE,     /* addr32 data8                         */

    /* prom */
    NSAE_CMD_PROM_LOAD,     /* buflen buf                           */

    /* mmu */
    NSAE_CMD_MMU_READ,      /* addr16 span16                        */
    NSAE_CMD_MMU_WRITE,     /* addr16 data8                         */
    NSAE_CMD_MMU_LOAD,      /* slot page                            */
    NSAE_CMD_MMU_STATUS,

    NSAE_CMD_COUNT,
} nsae_cmd_t;


#endif /* NSAECMD_H */
/* end of buf  */
