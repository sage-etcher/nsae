
#ifndef NSAECMD_H
#define NSAECMD_H


typedef enum {

    /* emulator */
    NSAE_CMD_EXIT,          /* 1     cmd                           */
    NSAE_CMD_RESTART,       /* 1     cmd                           */
    NSAE_CMD_PAUSE,         /* 1     cmd                           */
    NSAE_CMD_CONTINUE,      /* 1     cmd                           */
    NSAE_CMD_BREAKPOINT,    /* 3     cmd abs_addr                  */
    NSAE_CMD_STEP,          /* 1     cmd                           */
    NSAE_CMD_RUN,           /* 1     cmd                           */
    NSAE_CMD_STATUS,        /* 1     cmd                           */

    /* floppydisk */
    NSAE_CMD_FD_EJECT,      /* 2     cmd fd_num                    */
    NSAE_CMD_FD_LOAD,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_FD_SAVE,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_FD_BLK_READ,   /* 5     cmd fd_num side track sec     */
    NSAE_CMD_FD_STATUS,     /* 2     cmd fd_num                    */

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

    /* cpu */
    NSAE_CMD_CPU_STATUS,    /* 1     cmd                           */

    /* ram */
    NSAE_CMD_RAM_READ,      /* 5     cmd abs_addr                  */
    NSAE_CMD_RAM_WRITE,     /* 6     cmd abs_addr data             */

    /* prom */
    NSAE_CMD_PROM_LOAD,     /* 5/9+  cmd fielname_len file         */

    /* mmu */
    NSAE_CMD_MMU_READ,      /* 5     cmd addr                      */
    NSAE_CMD_MMU_WRITE,     /* 6     cmd addr data                 */
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

#endif /* NSAECMD_H */
/* end of file */
