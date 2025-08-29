
#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#ifndef usleep
int
usleep (unsigned long usec)
{
    int res = 0;
    struct timespec ts = (struct timespec){
        .tv_sec = usec / 1000000,
        .tv_nsec = (usec % 1000000) * 1000,
    };

    do {
        res = nanosleep (&ts, &ts);
    } while (res != 0);

    return res;
}
#endif

struct timeval
timeval_diff (struct timeval a, struct timeval b)
{
    struct timeval diff = { 0 };

    diff.tv_usec = a.tv_usec - b.tv_usec;
    diff.tv_sec  = a.tv_sec  - b.tv_sec;
    if (a.tv_usec < b.tv_usec)
    {
        diff.tv_sec--;
        diff.tv_usec += 1000000;
    }

    return diff;
}

struct timeval
timeval_sum (struct timeval a, struct timeval b)
{
    struct timeval sum = { 0 };

    sum.tv_usec = a.tv_usec + b.tv_usec;
    sum.tv_sec  = a.tv_sec + b.tv_sec;
    if ((1000000 - a.tv_usec) <= b.tv_usec)
    {
        a.tv_usec -= 1000000;
        a.tv_sec++;
    }

    return sum;
}

int
timeval_cmp (struct timeval a, struct timeval b)
{
    if (a.tv_sec > b.tv_sec) return 1;
    if (a.tv_sec < b.tv_sec) return -1;
    if (a.tv_usec > b.tv_usec) return 1;
    if (a.tv_usec < b.tv_usec) return -1;
    return 0;
}

int
timeval_between (struct timeval a, struct timeval min, struct timeval max)
{
    /* return ((min <= a) && (a < max)) */
    return ((timeval_cmp (min, a) <= 0) && (timeval_cmp (a, max) < 0));
}

/* end of file */
