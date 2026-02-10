
#ifndef NSAE_NSAE_H
#define NSAE_NSAE_H
#ifdef __cplusplus
extern "c" {
#endif

#include "adv.h"
#include "breakpoints.h"
#include "kb_decode.h"
#include "watchpoints.h"

#include <sc_map.h>

#include <stdbool.h>
#include <stdint.h>
#include <time.h>


#define NSAE_BREAKPOINT_MAX 256

typedef struct {
    adv_t adv;
    breakpoints_t br;
    watchpoints_t wp;
    struct sc_map_64v kbmap;

    int width;
    int height;
    int max_fps;
    float cycle_multiplier;
    float scale_multiplier;

    bool step;
    bool resuming;
    bool pause;
    bool exit;

    uint64_t frame_time;

    uint64_t fps_time;
    uint32_t fps_count;
    uint32_t fps;

    int frame_number;
} nsae_t;

typedef struct {
    char *socket_addr;
    char *disk_a;
    char *disk_b;
    int verbosity;
    int init_paused;
    int init_speaker;
    /* (64bit-only) 4bytes padding */
} nsae_config_t;

int nsae_start (nsae_t *self, nsae_config_t *config);

void nsae_status (nsae_t *self);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_NSAE_H */
/* end of file */
