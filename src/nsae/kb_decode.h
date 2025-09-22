
#ifndef KB_DECODE_H
#define KB_DECODE_H

#include "glinit.h"

#include <stdbool.h>
#include <stdint.h>


enum {
    KC_NO_MOD,
    KC_SHIFT   = 0x01,
    KC_CONTROL = 0x02,
    KC_CTRLSFT = 0x03,
    KC_COMMAND,
    KC_STATE_COUNT,
};

typedef struct {
    bool has_command;
    bool accepts_allcaps;
    bool accepts_cursorlock;
    uint8_t keycodes[KC_STATE_COUNT];
} key_t;

typedef struct {
    int key;
    key_t value;
} kbmap_hash_t;

typedef kbmap_hash_t kb_map_t;

kb_map_t *kbmap_init (void);
void kbmap_free (kb_map_t *self);

int kbmap_decode (kb_map_t *self, int key, int mod);

#endif
