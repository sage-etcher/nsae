
#ifndef NSAE_NSAE_H
#define NSAE_NSAE_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "adv.h"
#include "breakpoints.h"


#define NSAE_BREAKPOINT_MAX 256

typedef struct {
    adv_t adv;
    breakpoints_t br;

    int width;
    int height;
    int max_fps;

    bool step;
    bool resuming;
    bool pause;
    bool exit;

    struct timeval update_tv;
} nsae_t;

int nsae_start (nsae_t *self, int *p_argc, char **argv);

void nsae_status (nsae_t *self);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_NSAE_H */
/* end of file */
