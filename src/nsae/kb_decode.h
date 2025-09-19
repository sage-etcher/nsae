
#include <GLFW/glfw3.h>

static const int G_KEYMAP_SHIFT   = GLFW_MOD_SHIFT;
static const int G_KEYMAP_CONTROL = GLFW_MOD_CONTROL;
static const int G_KEYMAP_COMMAND = GLFW_MOD_ALT;

static const int G_KEYMAP_ALLCAPS = GLFW_MOD_CAPS_LOCK;
static const int G_KEYMAP_CURSOR_LOCK = GLFW_MOD_NUM_LOCK;


static const int G_KEYMAP_CODES[] = {
    GLFW_KEY_TAB,           1, 0x09, 0x09, 0x09, 0x09, 0x00, /* tab       */
    GLFW_KEY_ENTER,         1, 0x0d, 0x0d, 0x0d, 0x0d, 0x00, /* return    */
    GLFW_KEY_ESCAPE,        1, 0x1b, 0x1b, 0x1b, 0x1b, 0x00, /* esc       */
    GLFW_KEY_SPACE,         1, 0x20, 0x20, 0x20, 0x20, 0x00, /* space     */
    GLFW_KEY_APOSTROPHE,    1, 0x27, 0x22, 0x27, 0x22, 0x00, /* ' "       */
    GLFW_KEY_COMMA,         1, 0x2c, 0x3c, 0x2c, 0x3c, 0x00, /* , <       */
    GLFW_KEY_MINUS,         1, 0x2d, 0x5f, 0x2d, 0x1f, 0x00, /* - _       */
    GLFW_KEY_PERIOD,        1, 0x2e, 0x3e, 0x2e, 0x3e, 0x00, /* . >       */
    GLFW_KEY_SLASH,         1, 0x2f, 0x3f, 0x2f, 0x3f, 0x00, /* / ?       */
    GLFW_KEY_0,             1, 0x30, 0x29, 0x30, 0x29, 0x00, /* 0 )       */
    GLFW_KEY_1,             1, 0x31, 0x21, 0x31, 0x21, 0x00, /* 1 !       */
    GLFW_KEY_2,             1, 0x32, 0x40, 0x32, 0x00, 0x00, /* 2 @       */
    GLFW_KEY_3,             1, 0x33, 0x23, 0x33, 0x23, 0x00, /* 3 #       */
    GLFW_KEY_4,             1, 0x34, 0x24, 0x34, 0x24, 0x00, /* 4 $       */
    GLFW_KEY_5,             1, 0x35, 0x25, 0x35, 0x25, 0x00, /* 5 %       */
    GLFW_KEY_6,             1, 0x36, 0x5e, 0x35, 0x1e, 0x00, /* 6 ^       */
    GLFW_KEY_7,             1, 0x37, 0x26, 0x37, 0x26, 0x00, /* 7 &       */
    GLFW_KEY_8,             1, 0x38, 0x2a, 0x38, 0x2a, 0x00, /* 8 *       */
    GLFW_KEY_9,             1, 0x39, 0x28, 0x39, 0x28, 0x00, /* 9 (       */
    GLFW_KEY_SEMICOLON,     1, 0x3b, 0x3a, 0x3b, 0x3a, 0x00, /* ; :       */
    GLFW_KEY_EQUAL,         1, 0x3d, 0x2b, 0x3d, 0x2b, 0x00, /* = +       */
    GLFW_KEY_LEFT_BRACKET,  1, 0x5b, 0x7b, 0x1b, 0x7b, 0x00, /* [ {       */
    GLFW_KEY_RIGHT_BRACKET, 1, 0x5d, 0x7d, 0x1d, 0x7d, 0x00, /* ] }       */
    GLFW_KEY_A,             1, 0x61, 0x41, 0x01, 0x01, 0xc1, /* A         */
    GLFW_KEY_B,             1, 0x62, 0x42, 0x02, 0x02, 0xc2, /* B         */
    GLFW_KEY_C,             1, 0x63, 0x43, 0x03, 0x03, 0xc3, /* C         */
    GLFW_KEY_D,             1, 0x64, 0x44, 0x04, 0x04, 0xc4, /* D         */
    GLFW_KEY_E,             1, 0x65, 0x45, 0x05, 0x05, 0xc5, /* E         */
    GLFW_KEY_F,             1, 0x66, 0x46, 0x06, 0x06, 0xc6, /* F         */
    GLFW_KEY_G,             1, 0x67, 0x47, 0x07, 0x07, 0xc7, /* G         */
    GLFW_KEY_H,             1, 0x68, 0x48, 0x08, 0x08, 0xc8, /* H         */
    GLFW_KEY_I,             1, 0x69, 0x49, 0x09, 0x09, 0xc9, /* I         */
    GLFW_KEY_J,             1, 0x6a, 0x4a, 0x0a, 0x0a, 0xca, /* J         */
    GLFW_KEY_K,             1, 0x6b, 0x4b, 0x0b, 0x0b, 0xcb, /* K         */
    GLFW_KEY_L,             1, 0x6c, 0x4c, 0x0c, 0x0c, 0xcc, /* L         */
    GLFW_KEY_M,             1, 0x6d, 0x4d, 0x0d, 0x0d, 0xcd, /* M         */
    GLFW_KEY_N,             1, 0x6e, 0x4e, 0x0e, 0x0e, 0xce, /* N         */
    GLFW_KEY_O,             1, 0x6f, 0x4f, 0x0f, 0x0f, 0xcf, /* O         */
    GLFW_KEY_P,             1, 0x70, 0x50, 0x10, 0x10, 0xd0, /* P         */
    GLFW_KEY_Q,             1, 0x71, 0x51, 0x11, 0x11, 0xd1, /* Q         */
    GLFW_KEY_R,             1, 0x72, 0x52, 0x12, 0x12, 0xd2, /* R         */
    GLFW_KEY_S,             1, 0x73, 0x53, 0x13, 0x13, 0xd3, /* S         */
    GLFW_KEY_T,             1, 0x74, 0x54, 0x14, 0x14, 0xd4, /* T         */
    GLFW_KEY_U,             1, 0x75, 0x55, 0x15, 0x15, 0xd5, /* U         */
    GLFW_KEY_V,             1, 0x76, 0x56, 0x16, 0x16, 0xd6, /* V         */
    GLFW_KEY_W,             1, 0x77, 0x57, 0x17, 0x17, 0xd7, /* W         */
    GLFW_KEY_X,             1, 0x78, 0x58, 0x18, 0x18, 0xd8, /* X         */
    GLFW_KEY_Y,             1, 0x79, 0x59, 0x19, 0x19, 0xd9, /* Y         */
    GLFW_KEY_Z,             1, 0x7a, 0x5a, 0x1a, 0x1a, 0xda, /* Z         */
    GLFW_KEY_GRAVE_ACCENT,  1, 0x7c, 0x60, 0x7c, 0x60, 0x00, /* | `       */
    GLFW_KEY_BACKSLASH,     1, 0x7e, 0x5c, 0x7e, 0x1c, 0x00, /* ~ \       */
    GLFW_KEY_BACKSPACE,     1, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, /* backspace */
    GLFW_KEY_F1,            1, 0xdb, 0xea, 0xdb, 0xea, 0x9b, /* F1        */
    GLFW_KEY_F2,            1, 0xdc, 0xeb, 0xdc, 0xeb, 0x9c, /* F2        */
    GLFW_KEY_F3,            1, 0xdd, 0xec, 0xdd, 0xec, 0x9d, /* F3        */
    GLFW_KEY_F4,            1, 0xde, 0xed, 0xde, 0xed, 0x9e, /* F4        */
    GLFW_KEY_F5,            1, 0xdf, 0xee, 0xdf, 0xee, 0x9f, /* F5        */
    GLFW_KEY_F6,            1, 0xe0, 0xef, 0xe0, 0xef, 0xa0, /* F6        */
    GLFW_KEY_F7,            1, 0xe1, 0xf0, 0xe1, 0xf0, 0xa1, /* F7        */
    GLFW_KEY_F8,            1, 0xe2, 0xf1, 0xe2, 0xf1, 0xa2, /* F8        */
    GLFW_KEY_F9,            1, 0xe3, 0xf2, 0xe3, 0xf2, 0xa3, /* F9        */
    GLFW_KEY_F10,           1, 0xe4, 0xf3, 0xe4, 0xf3, 0xa4, /* F10       */
    GLFW_KEY_F11,           1, 0xe5, 0xf4, 0xe5, 0xf4, 0xa5, /* F11       */
    GLFW_KEY_F12,           1, 0xe6, 0xf5, 0xe6, 0xf5, 0xa6, /* F12       */
    GLFW_KEY_PRINT_SCREEN,  1, 0xe7, 0xf6, 0xe7, 0xf6, 0xa7, /* F13       */
    GLFW_KEY_SCROLL_LOCK,   1, 0xe8, 0xf7, 0xe8, 0xf7, 0xa8, /* F14       */
    GLFW_KEY_PAUSE,         1, 0xe9, 0xf8, 0xe9, 0xf8, 0xa9, /* F15       */

    GLFW_KEY_KP_ADD,        1, 0x2c  0xac, 0x8c, 0x8c, 0xab, /* ,         */
    GLFW_KEY_KP_SUBTRACT,   1, 0x2d  0xad, 0x8f, 0x8f, 0x80, /* -         */
    GLFW_KEY_KP_PERIOD,     1, 0x2e  0xae, 0x8e, 0x8e, 0x81, /* .         */
    GLFW_KEY_KP_0,          1, 0x30  0xb0, 0x90, 0x90, 0xc0, /* 0         */
    GLFW_KEY_KP_1,          1, 0x31  0x84, 0xb1, 0x91, 0xfa, /* 1         */
    GLFW_KEY_KP_2,          1, 0x32  0x8a, 0xb2, 0x92, 0xfb, /* 2         */
    GLFW_KEY_KP_3,          1, 0x33  0x83, 0xb3, 0x93, 0xfc, /* 3         */
    GLFW_KEY_KP_4,          1, 0x34  0x88, 0xb4, 0x94, 0xfd, /* 4         */
    GLFW_KEY_KP_5,          1, 0x35  0x85, 0xb5, 0x95, 0xba, /* 5         */
    GLFW_KEY_KP_6,          1, 0x36  0x86, 0xb6, 0x96, 0xbb, /* 6         */
    GLFW_KEY_KP_7,          1, 0x37  0x87, 0xb7, 0x97, 0xbc, /* 7         */
    GLFW_KEY_KP_8,          1, 0x38  0x82, 0xb8, 0x98, 0xbd, /* 8         */
    GLFW_KEY_KP_9,          1, 0x39  0x89, 0xb9, 0x99, 0xbe, /* 9         */

    GLFW_KEY_KP_ENTER,      1, 0x0d  0x8d, 0x9a, 0x9a, 0xaa, /* enter     */
};

#if 0
key         normal  shift   control ctrlsft cmnd
----------- ------- ------- ------- ------- ------- 
tab         09      09      09      09      -
return      0d      0d      0d      0d      -
esc         1b      1b      1b      1b      -
space       20      20      20      20      -
' "         27      22      27      22      -
, <         2c      3c      2c      3c      -
- _         2d      5f      2d      1f      -
. >         2e      3e      2e      3e      -
/ ?         2f      3f      2f      3f      -
0 )         30      29      30      29      -
1 !         31      21      31      21      -
2 @         32      40      32      00      -
3 #         33      23      33      23      -
4 $         34      24      34      24      -
5 %         35      25      35      25      -
6 ^         36      5e      35      1e      -
7 &         37      26      37      26      -
8 *         38      2a      38      2a      -
9 (         39      28      39      28      -
; :         3b      3a      3b      3a      -
= +         3d      2b      3d      2b      -
[ {         5b      7b      1b      7b      -
] }         5d      7d      1d      7d      -
A           61      41      01      01      c1
B           62      42      02      02      c2
C           63      43      03      03      c3
D           64      44      04      04      c4
E           65      45      05      05      c5
F           66      46      06      06      c6
G           67      47      07      07      c7
H           68      48      08      08      c8
I           69      49      09      09      c9
J           6a      4a      0a      0a      ca
K           6b      4b      0b      0b      cb
L           6c      4c      0c      0c      cc
M           6d      4d      0d      0d      cd
N           6e      4e      0e      0e      ce
O           6f      4f      0f      0f      cf
P           70      50      10      10      d0
Q           71      51      11      11      d1
R           72      52      12      12      d2
S           73      53      13      13      d3
T           74      54      14      14      d4
U           75      55      15      15      d5
V           76      56      16      16      d6
W           77      57      17      17      d7
X           78      58      18      18      d8
Y           79      59      19      19      d9
Z           7a      5a      1a      1a      da
| `         7c      60      7c      60      -
~ \         7e      5c      7e      1c      -
backspace   7f      7f      7f      7f      -
F1          db      ea      db      ea      9b
F2          dc      eb      dc      eb      9c
F3          dd      ec      dd      ec      9d
F4          de      ed      de      ed      9e
F5          df      ee      df      ee      9f
F6          e0      ef      e0      ef      a0
F7          e1      f0      e1      f0      a1
F8          e2      f1      e2      f1      a2
F9          e3      f2      e3      f2      a3
F10         e4      f3      e4      f3      a4
F11         e5      f4      e5      f4      a5
F12         e6      f5      e6      f5      a6
F13         e7      f6      e7      f6      a7
F14         e8      f7      e8      f7      a8
F15         e9      f8      e9      f8      a9

,           2c      ac      8c      8c      ab
-           2d      ad      8f      8f      80
.           2e      ae      8e      8e      81
0           30      b0      90      90      c0
1           31      84      b1      91      fa
2           32      8a      b2      92      fb
3           33      83      b3      93      fc
4           34      88      b4      94      fd
5           35      85      b5      95      ba
6           36      86      b6      96      bb
7           37      87      b7      97      bc
8           38      82      b8      98      bd
9           39      89      b9      99      be

enter       0d      8d      9a      9a      aa


#endif
