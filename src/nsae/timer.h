
#ifndef NSAE_TIMER_H
#define NSAE_TIMER_H

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifndef usleep
int usleep (unsigned long usec);
#endif

struct timeval timeval_diff (struct timeval a, struct timeval b);
struct timeval timeval_sum (struct timeval a, struct timeval b);
int timeval_cmp (struct timeval a, struct timeval b);
int timeval_between (struct timeval a, struct timeval min, struct timeval max);

#endif
/* end of file */
