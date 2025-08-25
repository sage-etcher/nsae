
#ifndef NSAECMD_H
#define NSAECMD_H


typedef enum {
    NSAE_CMD_EXIT,          /* 1     cmd                           */
    NSAE_CMD_RESTART,       /* 1     cmd                           */
    NSAE_CMD_PAUSE,         /* 1     cmd                           */
    NSAE_CMD_CONTINUE,      /* 1     cmd                           */
    NSAE_CMD_EJECT_FD,      /* 2     cmd fd_num                    */
    NSAE_CMD_LOAD_FD,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_SAVE_FD,       /* 6/10+ cmd fd_num filename_len file  */
    NSAE_CMD_EJECT_HD,      /* 1     cmd                           */
    NSAE_CMD_LOAD_HD,       /* 5/9+  cmd filename_len file         */
    NSAE_CMD_SAVE_HD,       /* 5/9+  cmd filename_len file         */
    NSAE_CMD_STEP,          /* 1     cmd                           */
    NSAE_CMD_RUN,           /* 1     cmd                           */
    NSAE_CMD_BREAKPOINT,    /* 5     cmd abs_addr                  */
    NSAE_CMD_STATUS,        /* 1     cmd                           */
    NSAE_CMD_COUNT,
} nsae_cmd_t;

/* type     name;           // bytes
 * uint8_t  cmd;            // 1
 * uint8_t  fd_num;         // 1
 * size_t   filename_len;   // 4/8
 * char     file[];         // n
 * uint32_t abs_addr;       // 4
 */

#endif /* NSAECMD_H */
/* end of file */
