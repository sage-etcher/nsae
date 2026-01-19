
#ifndef NSAE_CONFIG_H
#define NSAE_CONFIG_H

/* select forground colour */
#define CRT_FG_GREEN_P31
// #define CRT_FG_GREEN_P1
// #define CRT_FG_WHITE
// #define CRT_FG_LIGHT

#if defined(CRT_FG_GREEN_P31)
#   define CRT_FG_COLOR 0.38f, 1.0f, 0.0f   /* P31 (NorthStar Adv) #61ff00 */
#elif defined(CRT_FG_GREEN_P1)
#   define CRT_FG_COLOR 0.2f, 1.0f, 0.2f    /* P1 (apple II green) #33FF33 */
#elif defined(CRT_FG_WHITE)
#   define CRT_FG_COLOR 1.0f, 1.0f, 1.0f    /* White 0xFFFFFF */
#elif defined(CRT_FG_LIGHT)
#   define CRT_FG_COLOR 0.98f, 0.98f, 0.98f /* Light #FAFAFA */
#endif

/* select background colour */
#define CRT_BG_BLACK
// #define CRT_BG_DARK

#if defined(CRT_BG_BLACK)
#   define CRT_BG_COLOR 0.0f, 0.0f, 0.0f    /* Black 0x000000 */
#elif defined(CRT_BG_DARK)
#   define CRT_BG_COLOR 0.34f, 0.4f, 0.45f  /* Dark 0x5b6673 */
#endif

/* enable reversed */
// #define CRT_REVERSE

#if !defined(CRT_REVERSE)
#   define CRT_CLEAR_COLOR CRT_BG_COLOR
#   define CRT_DRAW_COLOR  CRT_FG_COLOR
#else
#   define CRT_CLEAR_COLOR CRT_FG_COLOR
#   define CRT_DRAW_COLOR  CRT_BG_COLOR
#endif

#endif
/* end of file */
