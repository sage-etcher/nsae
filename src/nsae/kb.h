
#ifndef NSAE_KB_H
#define NSAE_KB_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define KB_BUF_MAX 7

typedef struct {
    bool overflow;
    bool data_flag;
    bool reset;
    bool cursor_lock;
    bool caps_lock;
    bool autorepeat;
    uint8_t buf[KB_BUF_MAX];
    uint8_t buf_cnt;
} kb_t;

int kb_init (kb_t *self);

uint8_t kb_decode_key (kb_t *self, uint8_t host_key);

void kb_reset (kb_t *self);
void kb_push (kb_t *self, uint8_t key);
uint8_t kb_pop (kb_t *self);

uint8_t kb_get_lsb (kb_t *self);
uint8_t kb_get_msb (kb_t *self);

void kb_status (kb_t *self);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_KB_H */
/* end of file */
