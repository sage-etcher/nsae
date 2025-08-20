
#ifndef NSAE_NSAE_H
#define NSAE_NSAE_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "adv.h"


typedef struct {
    adv_t adv;

    int width;
    int height;
    int max_fps;

    bool pause;
    bool exit;
} nsae_t;

int nsae_start (nsae_t *self, int *p_argc, char **argv);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_NSAE_H */
/* end of file */
