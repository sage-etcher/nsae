
#ifndef NSAE_CPU_H
#define NSAE_CPU_H
#ifdef __cplusplus
extern "c" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    Z80STATE state;
} cpu_t;


#ifdef __cplusplus
}
#endif
#endif /* NSAE_CPU_H */
/* end of file */
