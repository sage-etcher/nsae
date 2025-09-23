
#include "kb_decode.h"

#include <sc_map.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARRLEN(a) (sizeof (a) / sizeof (*(a)))


struct sc_map_64v
kbmap_init (void)
{
    struct sc_map_64v map;
    static struct { int key; key_t value; } s_keymap_data[] = {
        { .key = GLFW_KEY_TAB,           .value = { false, false, false, { 0x09, 0x09, 0x09, 0x09, 0x00 } } }, /* tab       */
        { .key = GLFW_KEY_ENTER,         .value = { false, false, false, { 0x0d, 0x0d, 0x0d, 0x0d, 0x00 } } }, /* return    */
        { .key = GLFW_KEY_ESCAPE,        .value = { false, false, false, { 0x1b, 0x1b, 0x1b, 0x1b, 0x00 } } }, /* esc       */
        { .key = GLFW_KEY_SPACE,         .value = { false, false, false, { 0x20, 0x20, 0x20, 0x20, 0x00 } } }, /* space     */
        { .key = GLFW_KEY_APOSTROPHE,    .value = { false, false, false, { 0x27, 0x22, 0x27, 0x22, 0x00 } } }, /* ' "       */
        { .key = GLFW_KEY_COMMA,         .value = { false, false, false, { 0x2c, 0x3c, 0x2c, 0x3c, 0x00 } } }, /* , <       */
        { .key = GLFW_KEY_MINUS,         .value = { false, false, false, { 0x2d, 0x5f, 0x2d, 0x1f, 0x00 } } }, /* - _       */
        { .key = GLFW_KEY_PERIOD,        .value = { false, false, false, { 0x2e, 0x3e, 0x2e, 0x3e, 0x00 } } }, /* . >       */
        { .key = GLFW_KEY_SLASH,         .value = { false, false, false, { 0x2f, 0x3f, 0x2f, 0x3f, 0x00 } } }, /* / ?       */
        { .key = GLFW_KEY_0,             .value = { false, false, false, { 0x30, 0x29, 0x30, 0x29, 0x00 } } }, /* 0 )       */
        { .key = GLFW_KEY_1,             .value = { false, false, false, { 0x31, 0x21, 0x31, 0x21, 0x00 } } }, /* 1 !       */
        { .key = GLFW_KEY_2,             .value = { false, false, false, { 0x32, 0x40, 0x32, 0x00, 0x00 } } }, /* 2 @       */
        { .key = GLFW_KEY_3,             .value = { false, false, false, { 0x33, 0x23, 0x33, 0x23, 0x00 } } }, /* 3 #       */
        { .key = GLFW_KEY_4,             .value = { false, false, false, { 0x34, 0x24, 0x34, 0x24, 0x00 } } }, /* 4 $       */
        { .key = GLFW_KEY_5,             .value = { false, false, false, { 0x35, 0x25, 0x35, 0x25, 0x00 } } }, /* 5 %       */
        { .key = GLFW_KEY_6,             .value = { false, false, false, { 0x36, 0x5e, 0x35, 0x1e, 0x00 } } }, /* 6 ^       */
        { .key = GLFW_KEY_7,             .value = { false, false, false, { 0x37, 0x26, 0x37, 0x26, 0x00 } } }, /* 7 &       */
        { .key = GLFW_KEY_8,             .value = { false, false, false, { 0x38, 0x2a, 0x38, 0x2a, 0x00 } } }, /* 8 *       */
        { .key = GLFW_KEY_9,             .value = { false, false, false, { 0x39, 0x28, 0x39, 0x28, 0x00 } } }, /* 9 (       */
        { .key = GLFW_KEY_SEMICOLON,     .value = { false, false, false, { 0x3b, 0x3a, 0x3b, 0x3a, 0x00 } } }, /* ; :       */
        { .key = GLFW_KEY_EQUAL,         .value = { false, false, false, { 0x3d, 0x2b, 0x3d, 0x2b, 0x00 } } }, /* = +       */
        { .key = GLFW_KEY_LEFT_BRACKET,  .value = { false, false, false, { 0x5b, 0x7b, 0x1b, 0x7b, 0x00 } } }, /* [ {       */
        { .key = GLFW_KEY_RIGHT_BRACKET, .value = { false, false, false, { 0x5d, 0x7d, 0x1d, 0x7d, 0x00 } } }, /* ] }       */
        { .key = GLFW_KEY_A,             .value = { true,  true,  false, { 0x61, 0x41, 0x01, 0x01, 0xc1 } } }, /* A         */
        { .key = GLFW_KEY_B,             .value = { true,  true,  false, { 0x62, 0x42, 0x02, 0x02, 0xc2 } } }, /* B         */
        { .key = GLFW_KEY_C,             .value = { true,  true,  false, { 0x63, 0x43, 0x03, 0x03, 0xc3 } } }, /* C         */
        { .key = GLFW_KEY_D,             .value = { true,  true,  false, { 0x64, 0x44, 0x04, 0x04, 0xc4 } } }, /* D         */
        { .key = GLFW_KEY_E,             .value = { true,  true,  false, { 0x65, 0x45, 0x05, 0x05, 0xc5 } } }, /* E         */
        { .key = GLFW_KEY_F,             .value = { true,  true,  false, { 0x66, 0x46, 0x06, 0x06, 0xc6 } } }, /* F         */
        { .key = GLFW_KEY_G,             .value = { true,  true,  false, { 0x67, 0x47, 0x07, 0x07, 0xc7 } } }, /* G         */
        { .key = GLFW_KEY_H,             .value = { true,  true,  false, { 0x68, 0x48, 0x08, 0x08, 0xc8 } } }, /* H         */
        { .key = GLFW_KEY_I,             .value = { true,  true,  false, { 0x69, 0x49, 0x09, 0x09, 0xc9 } } }, /* I         */
        { .key = GLFW_KEY_J,             .value = { true,  true,  false, { 0x6a, 0x4a, 0x0a, 0x0a, 0xca } } }, /* J         */
        { .key = GLFW_KEY_K,             .value = { true,  true,  false, { 0x6b, 0x4b, 0x0b, 0x0b, 0xcb } } }, /* K         */
        { .key = GLFW_KEY_L,             .value = { true,  true,  false, { 0x6c, 0x4c, 0x0c, 0x0c, 0xcc } } }, /* L         */
        { .key = GLFW_KEY_M,             .value = { true,  true,  false, { 0x6d, 0x4d, 0x0d, 0x0d, 0xcd } } }, /* M         */
        { .key = GLFW_KEY_N,             .value = { true,  true,  false, { 0x6e, 0x4e, 0x0e, 0x0e, 0xce } } }, /* N         */
        { .key = GLFW_KEY_O,             .value = { true,  true,  false, { 0x6f, 0x4f, 0x0f, 0x0f, 0xcf } } }, /* O         */
        { .key = GLFW_KEY_P,             .value = { true,  true,  false, { 0x70, 0x50, 0x10, 0x10, 0xd0 } } }, /* P         */
        { .key = GLFW_KEY_Q,             .value = { true,  true,  false, { 0x71, 0x51, 0x11, 0x11, 0xd1 } } }, /* Q         */
        { .key = GLFW_KEY_R,             .value = { true,  true,  false, { 0x72, 0x52, 0x12, 0x12, 0xd2 } } }, /* R         */
        { .key = GLFW_KEY_S,             .value = { true,  true,  false, { 0x73, 0x53, 0x13, 0x13, 0xd3 } } }, /* S         */
        { .key = GLFW_KEY_T,             .value = { true,  true,  false, { 0x74, 0x54, 0x14, 0x14, 0xd4 } } }, /* T         */
        { .key = GLFW_KEY_U,             .value = { true,  true,  false, { 0x75, 0x55, 0x15, 0x15, 0xd5 } } }, /* U         */
        { .key = GLFW_KEY_V,             .value = { true,  true,  false, { 0x76, 0x56, 0x16, 0x16, 0xd6 } } }, /* V         */
        { .key = GLFW_KEY_W,             .value = { true,  true,  false, { 0x77, 0x57, 0x17, 0x17, 0xd7 } } }, /* W         */
        { .key = GLFW_KEY_X,             .value = { true,  true,  false, { 0x78, 0x58, 0x18, 0x18, 0xd8 } } }, /* X         */
        { .key = GLFW_KEY_Y,             .value = { true,  true,  false, { 0x79, 0x59, 0x19, 0x19, 0xd9 } } }, /* Y         */
        { .key = GLFW_KEY_Z,             .value = { true,  true,  false, { 0x7a, 0x5a, 0x1a, 0x1a, 0xda } } }, /* Z         */
        { .key = GLFW_KEY_GRAVE_ACCENT,  .value = { false, false, false, { 0x7c, 0x60, 0x7c, 0x60, 0x00 } } }, /* | `       */
        { .key = GLFW_KEY_BACKSLASH,     .value = { false, false, false, { 0x7e, 0x5c, 0x7e, 0x1c, 0x00 } } }, /* ~ \       */
        { .key = GLFW_KEY_BACKSPACE,     .value = { false, false, false, { 0x7f, 0x7f, 0x7f, 0x7f, 0x00 } } }, /* backspace */
        { .key = GLFW_KEY_F1,            .value = { true,  false, false, { 0xdb, 0xea, 0xdb, 0xea, 0x9b } } }, /* F1        */
        { .key = GLFW_KEY_F2,            .value = { true,  false, false, { 0xdc, 0xeb, 0xdc, 0xeb, 0x9c } } }, /* F2        */
        { .key = GLFW_KEY_F3,            .value = { true,  false, false, { 0xdd, 0xec, 0xdd, 0xec, 0x9d } } }, /* F3        */
        { .key = GLFW_KEY_F4,            .value = { true,  false, false, { 0xde, 0xed, 0xde, 0xed, 0x9e } } }, /* F4        */
        { .key = GLFW_KEY_F5,            .value = { true,  false, false, { 0xdf, 0xee, 0xdf, 0xee, 0x9f } } }, /* F5        */
        { .key = GLFW_KEY_F6,            .value = { true,  false, false, { 0xe0, 0xef, 0xe0, 0xef, 0xa0 } } }, /* F6        */
        { .key = GLFW_KEY_F7,            .value = { true,  false, false, { 0xe1, 0xf0, 0xe1, 0xf0, 0xa1 } } }, /* F7        */
        { .key = GLFW_KEY_F8,            .value = { true,  false, false, { 0xe2, 0xf1, 0xe2, 0xf1, 0xa2 } } }, /* F8        */
        { .key = GLFW_KEY_F9,            .value = { true,  false, false, { 0xe3, 0xf2, 0xe3, 0xf2, 0xa3 } } }, /* F9        */
        { .key = GLFW_KEY_F10,           .value = { true,  false, false, { 0xe4, 0xf3, 0xe4, 0xf3, 0xa4 } } }, /* F10       */
        { .key = GLFW_KEY_F11,           .value = { true,  false, false, { 0xe5, 0xf4, 0xe5, 0xf4, 0xa5 } } }, /* F11       */
        { .key = GLFW_KEY_F12,           .value = { true,  false, false, { 0xe6, 0xf5, 0xe6, 0xf5, 0xa6 } } }, /* F12       */
        { .key = GLFW_KEY_PRINT_SCREEN,  .value = { true,  false, false, { 0xe7, 0xf6, 0xe7, 0xf6, 0xa7 } } }, /* F13       */
        { .key = GLFW_KEY_SCROLL_LOCK,   .value = { true,  false, false, { 0xe8, 0xf7, 0xe8, 0xf7, 0xa8 } } }, /* F14       */
        { .key = GLFW_KEY_PAUSE,         .value = { true,  false, false, { 0xe9, 0xf8, 0xe9, 0xf8, 0xa9 } } }, /* F15       */

        { .key = GLFW_KEY_KP_ADD,        .value = { true,  false, false, { 0x2c, 0xac, 0x8c, 0x8c, 0xab } } }, /* ,         */
        { .key = GLFW_KEY_KP_SUBTRACT,   .value = { true,  false, false, { 0x2d, 0xad, 0x8f, 0x8f, 0x80 } } }, /* -         */
        { .key = GLFW_KEY_KP_DECIMAL,    .value = { true,  false, false, { 0x2e, 0xae, 0x8e, 0x8e, 0x81 } } }, /* .         */
        { .key = GLFW_KEY_KP_0,          .value = { true,  false, true,  { 0x30, 0xb0, 0x90, 0x90, 0xc0 } } }, /* 0         */
        { .key = GLFW_KEY_KP_1,          .value = { true,  false, true,  { 0x31, 0x84, 0xb1, 0x91, 0xfa } } }, /* 1         */
        { .key = GLFW_KEY_KP_2,          .value = { true,  false, true,  { 0x32, 0x8a, 0xb2, 0x92, 0xfb } } }, /* 2         */
        { .key = GLFW_KEY_KP_3,          .value = { true,  false, true,  { 0x33, 0x83, 0xb3, 0x93, 0xfc } } }, /* 3         */
        { .key = GLFW_KEY_KP_4,          .value = { true,  false, true,  { 0x34, 0x88, 0xb4, 0x94, 0xfd } } }, /* 4         */
        { .key = GLFW_KEY_KP_5,          .value = { true,  false, true,  { 0x35, 0x85, 0xb5, 0x95, 0xba } } }, /* 5         */
        { .key = GLFW_KEY_KP_6,          .value = { true,  false, true,  { 0x36, 0x86, 0xb6, 0x96, 0xbb } } }, /* 6         */
        { .key = GLFW_KEY_KP_7,          .value = { true,  false, true,  { 0x37, 0x87, 0xb7, 0x97, 0xbc } } }, /* 7         */
        { .key = GLFW_KEY_KP_8,          .value = { true,  false, true,  { 0x38, 0x82, 0xb8, 0x98, 0xbd } } }, /* 8         */
        { .key = GLFW_KEY_KP_9,          .value = { true,  false, true,  { 0x39, 0x89, 0xb9, 0x99, 0xbe } } }, /* 9         */

        { .key = GLFW_KEY_KP_ENTER,      .value = { true,  false, false, { 0x0d, 0x8d, 0x9a, 0x9a, 0xaa } } }, /* enter     */
    };

    sc_map_init_64v (&map, 0, 0);

    for (size_t i = 0; i < ARRLEN (s_keymap_data); i++)
    {
        sc_map_put_64v (&map, s_keymap_data[i].key, (void *)&s_keymap_data[i].value);
    }

    return map;
}


void
kbmap_free (struct sc_map_64v *self)
{
    sc_map_term_64v (self);
}


int
kbmap_decode (struct sc_map_64v *self, int key, int mods)
{
    /* get match */
    key_t *p_keycode = NULL;
    bool match = sc_map_get_64v (self, key, (void **)&p_keycode);
    if (!match) return -1;

    /* get mod_index */
    int mod_index = 0;
    if (((p_keycode->accepts_allcaps) && (mods & GLFW_MOD_CAPS_LOCK)) ||
        ((p_keycode->accepts_cursorlock) && (mods & GLFW_MOD_NUM_LOCK)))
    {
        mod_index |= KC_SHIFT;
    }

    if (mods & GLFW_MOD_SHIFT)
    {
        mod_index ^= KC_SHIFT;
    }

    if (mods & GLFW_MOD_CONTROL)
    {
        mod_index |= KC_CONTROL;
    }

    if (p_keycode->has_command && (mods & GLFW_MOD_ALT))
    {
        mod_index = KC_COMMAND;
    }

    /* get keycode */
    uint8_t keycode = p_keycode->keycodes[mod_index];

    return keycode;
}



/* end of file */
