
#ifndef NSAE_SPEAKER_H
#define NSAE_SPEAKER_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool unimplimented;
} speaker_t;

int speaker_init (speaker_t *self);

void speaker_beep (speaker_t *self);

#ifdef __cplusplus
}
#endif
#endif /* NSAE_SPEAKER_H */
/* end of file */
