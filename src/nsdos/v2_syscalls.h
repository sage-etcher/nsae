
#include <stdint.h>

#define ATTRIBUTE_PACKED __attribute__((packed))

enum {
    TYPE_DEFAULT    = 0,
    TYPE_BINARY     = 1,
    TYPE_BASIC_PROG = 2,
    TYPE_BASIC_DATA = 3,
    TYPE_OTHER      = 4,
};

struct file_entry {
    uint8_t symbol_name[8];
    uint16_t disk_address;
    uint16_t number_of_block;
    uint8_t file_type;
    union {
        uint8_t basic_valid_data_count;
        uint16_t go_memory_address;
        uint8_t padding[3];
    } extra;
} ATTRIBUTE_PACKED; /* 16 bytes */

/* north star disk operating system
 *
 *      ORG 2000h       standard version origin value
 *      DS 13           these cells are reserved
 */

/* BIOS Routines */
/* This is the character output routine
 * the character to be output must be in the b register.
 * device number may be supplied in acc, if desired.
 * on return the character must also be in the acc.
 * no other registers may be modified. */
uint8_t cout(uint8_t character, uint8_t device) = 0x200d;

/* This is the character input routine.
 * device number may be supplied in acc, if desired.
 * the 7-bit ascii code must be returned in the acc.
 * no other registers may be modified */
uint8_t cin(uint8_t device) = 0x2010;

/* This is the terminal initialization routine
 * all registers may be used.
 * if not needed, merely patch in a ret. */
void tinit(void) = 0x2013;

/* This routine detects a Control-C
 * if Z is set on return, that means a control-c was typed.
 * otherwise, if no character was typoed or a character other 
 * than control-c was typed, z must not be set.
 * contc should return immediately if no char was typed,
 * not wait for a character and then return.
 * all registers may be used. */
bool contc(void) = 0x2016;


/* DOS Library routine entry points */

/* This address is branched to on hard disk errors */
void hderr(void) = 0x2019;

/* This is the file directory lookup routine
 * acc must contain the default unit number (normally 1)
 * HL=pointer to file name in ram,
 * followed by either a blank or carriaage return.
 * failure if carry set. On failure, HL=first free disk address
 * on success, acc=the disk unit indicated, and HL has a pointer
 * to the eighth byte of a copy of the entry in dos ram */
struct { bool cy; void *disk_address; uint8_t unit; } dlook(uint8_t def_unit, 
        uint8_t *filename) = 0x201c;

/* this routine ill write a directory entry back to disk
 * no args are needed. must follow dlook */
void dwrit(void) = 0x201f;

/* this routine may be used to issue a disk command
 * ACC=number of blocks
 * B=command (0=write, 1=read, 2=verify
 * C=unit number
 * DE=starting ram address
 * HL=starting disk address
 * return with carry set means arguements were illegal */
struct { bool cy; } dcom(uint8_t blocks, uint8_t command, uint8_t unit_number, 
        void *ram_address, uint16_t disk_address) = 0x2022;

/* this routine may be used to list a file directory
 * acc=disk unit */
void list(uint8_t disk_unit) = 0x2025;

/* this address is an entry point to the loaded dos */
void dos(void) = 0x2028;

/* this next bye is a flag used by dos.
 * if 0, then read-after-write check is not done,
 * if 1, then read-after-write check is done. */
uint8_t rwchk = 0;

/* north star basic, version 6
                ORG 2A00H       ;standard version origin
    
2A00    EP0:    XRA A           ;initialization entry point
                JMP EP11
2A04    EP1:    STC             ;continue entry point
2A05    EP11:   LXI H,ENDBAS    ;first cell of program region
                LXI D,5FFFH     ;last cell of contigous memory
                JMP START
 */






