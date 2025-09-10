
#ifndef NSAECMD_H
#define NSAECMD_H


typedef enum {

    /* emulator */
    NSAE_CMD_NOP,           /* 1     cmd                           */
    NSAE_CMD_EXIT,          /* 1     cmd                           */
    NSAE_CMD_RESTART,       /* 1     cmd                           */
    NSAE_CMD_PAUSE,         /* 1     cmd                           */
    NSAE_CMD_CONTINUE,      /* 1     cmd                           */
    NSAE_CMD_BRKPNT_SET,    /* 3     cmd abs_addr                  */
    NSAE_CMD_BRKPNT_REMOVE, /* 3     cmd abs_addr                  */
    NSAE_CMD_BRKPNT_LIST,   /* 1     cmd                           */
    NSAE_CMD_STEP,          /* 1     cmd                           */
    NSAE_CMD_RUN,           /* 1     cmd                           */
    NSAE_CMD_STATUS,        /* 1     cmd                           */

    /* logging */
    NSAE_CMD_LOG_CPU,       /* 2     cmd state                     */
    NSAE_CMD_LOG_MMU,       /* 2     cmd state                     */
    NSAE_CMD_LOG_RAM,       /* 2     cmd state                     */
    NSAE_CMD_LOG_FDC,       /* 2     cmd state                     */
    NSAE_CMD_LOG_CRT,       /* 2     cmd state                     */
    NSAE_CMD_LOG_KB,        /* 2     cmd state                     */
    NSAE_CMD_LOG_MOBO,      /* 2     cmd state                     */
    NSAE_CMD_LOG_TERSE,     /* 1     cmd                           */
    NSAE_CMD_LOG_VERBOSE,   /* 1     cmd                           */
    NSAE_CMD_LOG_DEBUG,     /* 1     cmd                           */
    NSAE_CMD_LOG_OUTPUT,    /* 5/9+  cmd filename_len file         */

    /* floppydisk */
    NSAE_CMD_FD_EJECT,      /* 2     cmd fd_num                    */
    NSAE_CMD_FD_LOAD,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_FD_SAVE,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_FD_BLK_READ,   /* 5     cmd fd_num side track sec     */
    NSAE_CMD_FD_STATUS,     /* 1     cmd                           */

    /* harddisk */
    NSAE_CMD_HD_EJECT,      /* 1     cmd                           */
    NSAE_CMD_HD_LOAD,       /* 5/9+  cmd filename_len file         */
    NSAE_CMD_HD_SAVE,       /* 5/9+  cmd filename_len file         */
    NSAE_CMD_HD_STATUS,     /* 1     cmd                           */

    /* keyboard */
    NSAE_CMD_KB_PUSH,       /* 2     cmd keycode                   */
    NSAE_CMD_KB_POP,        /* 1     cmd                           */
    NSAE_CMD_KB_OVERFLOW,   /* 2     cmd state                     */
    NSAE_CMD_KB_CAPS,       /* 2     cmd state                     */
    NSAE_CMD_KB_CURSOR,     /* 2     cmd state                     */
    NSAE_CMD_KB_DATA,       /* 2     cmd state                     */
    NSAE_CMD_KB_INTERUPT,   /* 2     cmd state                     */
    NSAE_CMD_KB_STATUS,     /* 1     cmd                           */

    /* io boards */
    NSAE_CMD_IO_STATUS,     /* 1     cmd                           */

    /* display */
    NSAE_CMD_CRT_STATUS,    /* 1     cmd                           */

    /* advantage */
    NSAE_CMD_ADV_STATUS,    /* 1     cmd                           */
    NSAE_CMD_ADV_OUT,       /* 3     cmd port data                 */
    NSAE_CMD_ADV_IN,        /* 2     cmd port                      */

    /* cpu */
    NSAE_CMD_CPU_STATUS,    /* 1     cmd                           */

    /* ram */
    NSAE_CMD_RAM_READ,      /* 9/13  cmd abs_addr len              */
    NSAE_CMD_RAM_WRITE,     /* 6     cmd abs_addr data             */

    /* prom */
    NSAE_CMD_PROM_LOAD,     /* 5/9+  cmd fielname_len file         */

    /* mmu */
    NSAE_CMD_MMU_READ,      /* 7/11  cmd addr len                  */
    NSAE_CMD_MMU_WRITE,     /* 4     cmd addr data                 */
    NSAE_CMD_MMU_LOAD,      /* 3     cmd slot page                 */
    NSAE_CMD_MMU_STATUS,    /* 1     cmd                           */

    NSAE_CMD_COUNT,
} nsae_cmd_t;

/* type     name;           // bytes
 * uint8_t  cmd;            // 1
 * uint8_t  fd_num;         // 1
 * size_t   filename_len;   // 4/8
 * char     file[];         // n
 * uint32_t abs_addr;       // 4
 * uint16_t addr;           // 2
 * uint8_t  data;           // 1
 * uint8_t  page;           // 1
 * uint8_t  state;          // 1
 * uint8_t  slot;           // 1
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t length;
    char *buf;
} nscmd_str_t;


typedef union {
    bool b;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;

} nscmd_args_t;



#endif /* NSAECMD_H */
/* end of file */
