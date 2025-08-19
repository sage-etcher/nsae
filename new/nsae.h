
#ifndef NSAE_NSAE_H
#define NSAE_NSAE_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "ade.h"


typedef struct {
    adv_t adv;

    int width;
    int height;
    int max_fps;

    bool pause;
    bool exit;
} nsae_t;

nsae_t *nsae_new (void);
void nsae_free (nsae_t *self);

int nsae_start (nsae_t *self);


#ifdef __cplusplus
}
#endif
#endif /* NSAE_NSAE_H */
/* end of file */
