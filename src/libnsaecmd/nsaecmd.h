
#ifndef NSAECMD_H
#define NSAECMD_H

#include <stdint.h>

/*  virtual "union" sketch
 *   0      packet_size
 *   1      "
 *   2      "
 *   3      "
 *   4      cmd
 *   5      mode
 *   6      var
 *   7
 *   8      span32  data    data    fd_num      slot    state   keycode loglevel    data16
 *   9      "               port    fd_side     page                                "
 *  10      "               span16  fd_track                                        wp_type
 *  11      "               "       fd_sector                                       wp_match
 *  12      addr32  addr32  addr16  buflen                                          addr32
 *  13      "               "       "                                               "
 *  14      "                       "                                               "
 *  15      "                       "                                               "
 *  16+     abstract buffer data
 */

typedef struct {
    uint8_t cmd;
    uint8_t mode;
    uint8_t var;
    uint8_t x;  /* unused padding */
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
#define v_data32    a.u32       /* 0 - 3    */
#define v_addr32    a.u32       /* 0 - 3    */

#define v_data8     b.u8[0]     /* 4 */
#define v_format    b.u8[1]     /* 5 */
#define v_fddrive   b.u8[1]     /* 5 */
#define v_index     b.u16[1]    /* 6 - 7 */
#define v_count     b.u16[1]    /* 6 - 7 */


/* cmd types */
typedef enum {
    NSAE_CMD_NULL,
    NSAE_CMD_QUIT = 0xf0,
    NSAE_CMD_RUN,
    NSAE_CMD_STEP,
    NSAE_CMD_NEXT,
    NSAE_CMD_CONTINUE,
    NSAE_CMD_INFO,
    NSAE_CMD_SET,
    NSAE_CMD_DELETE,
    NSAE_CMD_LOAD,
    NSAE_CMD_SAVE,
    NSAE_CMD_READ,
    NSAE_CMD_WRITE,
    NSAE_CMD_EXTRA,
} nsae_cmd_t;

typedef enum {
    NSAE_MODE_NULL = 0x00,
    NSAE_MODE_NSAE,
    NSAE_MODE_BREAKPOINT,
    NSAE_MODE_ADVANTAGE,
    NSAE_MODE_CPU,
    NSAE_MODE_DISPLAY,
    NSAE_MODE_FLOPPY,
    NSAE_MODE_HARDDRIVE,
    NSAE_MODE_PROM,
    NSAE_MODE_KEYBOARD,
    NSAE_MODE_MMU,
    NSAE_MODE_RAM,
    NSAE_MODE_LOG,
    NSAE_MODE_ALL,
} nsae_mode_t;

typedef enum {
    NSAE_VAR_NULL = 0x00,
    NSAE_VAR_BREAKPOINT_APPEND,

    NSAE_VAR_ADVANTAGE_KBMI,
    NSAE_VAR_ADVANTAGE_KBNMI,
    NSAE_VAR_ADVANTAGE_CRTMI,
    NSAE_VAR_ADVANTAGE_INTERUPT,
    NSAE_VAR_ADVANTAGE_CMDACK,

    // NSAE_VAR_PORT,

    NSAE_VAR_CPU_A,
    NSAE_VAR_CPU_BC,
    NSAE_VAR_CPU_DE,
    NSAE_VAR_CPU_HL,
    NSAE_VAR_CPU_PC,
    NSAE_VAR_CPU_SP,
    NSAE_VAR_CPU_IX,
    NSAE_VAR_CPU_IY,
    NSAE_VAR_CPU_I,
    NSAE_VAR_CPU_R,
    NSAE_VAR_CPU_IFF1,
    NSAE_VAR_CPU_IFF2,
    NSAE_VAR_CPU_IM,
    NSAE_VAR_CPU_EXX,
    NSAE_VAR_CPU_HALT,
    NSAE_VAR_CPU_S_FLAG,
    NSAE_VAR_CPU_Z_FLAG,
    NSAE_VAR_CPU_H_FLAG,
    NSAE_VAR_CPU_P_FLAG,
    NSAE_VAR_CPU_V_FLAG,
    NSAE_VAR_CPU_N_FLAG,
    NSAE_VAR_CPU_C_FLAG,

    NSAE_VAR_DISPLAY_COLOR,
    NSAE_VAR_DISPLAY_BLANK,
    NSAE_VAR_DISPLAY_VSYNC,
    NSAE_VAR_DISPLAY_SCROLL,

    NSAE_VAR_FLOPPY_DISK,
    NSAE_VAR_FLOPPY_SIDE,
    NSAE_VAR_FLOPPY_TRACK,
    NSAE_VAR_FLOPPY_POWERED,
    NSAE_VAR_FLOPPY_TRACKZERO,
    NSAE_VAR_FLOPPY_SECTORMARK,
    NSAE_VAR_FLOPPY_EJECT_A,
    NSAE_VAR_FLOPPY_EJECT_B,
    NSAE_VAR_FLOPPY_SECTOR_A,
    NSAE_VAR_FLOPPY_SECTOR_B,

    NSAE_VAR_HARDDRIVE_EJECT,

    NSAE_VAR_KEYBOARD_REPEAT,
    NSAE_VAR_KEYBOARD_CAPSLOCK,
    NSAE_VAR_KEYBOARD_CURSORLOCK,
    NSAE_VAR_KEYBOARD_OVERFLOW,
    NSAE_VAR_KEYBOARD_DATAFLAG,
    NSAE_VAR_KEYBOARD_INTERRUPT,
    NSAE_VAR_KEYBOARD_PRESS,

    NSAE_VAR_MMU_SLOT0,
    NSAE_VAR_MMU_SLOT1,
    NSAE_VAR_MMU_SLOT2,
    NSAE_VAR_MMU_SLOT3,

    NSAE_VAR_LOG_DISPLAY,
    NSAE_VAR_LOG_CPU,
    NSAE_VAR_LOG_KEYBOARD,
    NSAE_VAR_LOG_RAM,
    NSAE_VAR_LOG_MMU,
    NSAE_VAR_LOG_FLOPPY,
    NSAE_VAR_LOG_HARDDRIVE,
    NSAE_VAR_LOG_ADVANTAGE,
    NSAE_VAR_LOG_ALL,
    NSAE_VAR_LOG_OUTPUT_FILE,
} nsae_var_t;


#endif /* NSAECMD_H */
/* end of buf  */
