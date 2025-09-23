
#ifndef KB_DECODE_H
#define KB_DECODE_H

#include "glinit.h"

#include <sc_map.h>

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

struct sc_map_64v kbmap_init (void);
void kbmap_free (struct sc_map_64v *self);

int kbmap_decode (struct sc_map_64v *self, int key, int mod);

#endif
