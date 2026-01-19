
#ifndef NSAECMD_H
#define NSAECMD_H

#include <stdint.h>

/*  virtual "union" sketch
 *   0      packet_size
 *   1      "
 *   2      "
 *   3      "
 *   4      cmd
 *   5      var
 *   6      mode
 *   7      "
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
    uint8_t var;
    uint16_t mode;
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
    CMD_NULL,
    CMD_QUIT,
    CMD_RUN,
    CMD_STEP,
    CMD_NEXT,
    CMD_INFO,
    CMD_SET,
    CMD_DELETE,
    CMD_LOAD,
    CMD_SAVE,
    CMD_READ,
    CMD_WRITE,
    CMD_IMP_BASE,
} nsae_cmd_t;

typedef enum {
    MODE_NULL = 0x0000,
    MODE_ADV  = 0x0001,
    MODE_BR   = 0x0002,
    MODE_CPU  = 0x0004,
    MODE_CRT  = 0x0008,
    MODE_FDC  = 0x0010,
    MODE_HDC  = 0x0020,
    MODE_IO   = 0x0040,
    MODE_KB   = 0x0080,
    MODE_LOG  = 0x0100,
    MODE_MMU  = 0x0200,
    MODE_NSAE = 0x0400,
    MODE_PROM = 0x0800,
    MODE_RAM  = 0x1000,
    MODE_WP   = 0x2000,
    MODE_ALL  = 0x4000,
} nsae_mode_t;

typedef enum {
    VAR_NULL = 0x00,
    VAR_BR_APPEND,

    VAR_ADV_KBMI,
    VAR_ADV_KBNMI,
    VAR_ADV_CRTMI,
    VAR_ADV_INTERUPT,
    VAR_ADV_CMDACK,

    // VAR_PORT,

    VAR_CPU_A,
    VAR_CPU_BC,
    VAR_CPU_DE,
    VAR_CPU_HL,
    VAR_CPU_PC,
    VAR_CPU_SP,
    VAR_CPU_IX,
    VAR_CPU_IY,
    VAR_CPU_I,
    VAR_CPU_R,
    VAR_CPU_IFF1,
    VAR_CPU_IFF2,
    VAR_CPU_IM,
    VAR_CPU_EXX,
    VAR_CPU_HALT,
    VAR_CPU_S_FLAG,
    VAR_CPU_Z_FLAG,
    VAR_CPU_H_FLAG,
    VAR_CPU_P_FLAG,
    VAR_CPU_V_FLAG,
    VAR_CPU_N_FLAG,
    VAR_CPU_C_FLAG,

    VAR_CRT_COLOR,
    VAR_CRT_BLANK,
    VAR_CRT_VSYNC,
    VAR_CRT_SCROLL,

    VAR_FDC_DISK,
    VAR_FDC_SIDE,
    VAR_FDC_TRACK,
    VAR_FDC_POWERED,
    VAR_FDC_TRACKZERO,
    VAR_FDC_SECTORMARK,
    VAR_FDC_EJECT_A,
    VAR_FDC_EJECT_B,
    VAR_FDC_SECTOR_A,
    VAR_FDC_SECTOR_B,

    VAR_HDC_EJECT,

    VAR_KB_REPEAT,
    VAR_KB_CAPSLOCK,
    VAR_KB_CURSORLOCK,
    VAR_KB_OVERFLOW,
    VAR_KB_DATAFLAG,
    VAR_KB_INTERRUPT,
    VAR_KB_PRESS,

    VAR_MMU_SLOT0,
    VAR_MMU_SLOT1,
    VAR_MMU_SLOT2,
    VAR_MMU_SLOT3,

    VAR_LOG_CRT,
    VAR_LOG_CPU,
    VAR_LOG_KB,
    VAR_LOG_RAM,
    VAR_LOG_MMU,
    VAR_LOG_FDC,
    VAR_LOG_HDC,
    VAR_LOG_ADV,
    VAR_LOG_ALL,
    VAR_LOG_OUTPUT_FILE,
} nsae_var_t;


#endif /* NSAECMD_H */
/* end of buf  */
