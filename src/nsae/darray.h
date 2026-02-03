
#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>

#define DARRAY_T(t)                                                         \
    struct {                                                                \
        size_t elem_size;                                                   \
        size_t alloc;                                                       \
        size_t count;                                                       \
        t *m;                                                               \
    }

#define DA_INIT(var,t) do {                                                 \
        var.elem_size = sizeof (t);                                         \
        var.alloc = 0;                                                      \
        var.count = 0;                                                      \
        var.m = NULL;                                                       \
    } while (0)

typedef DARRAY_T (void) da_void_t;


da_void_t *da_pop_front (da_void_t *self, int n);
da_void_t *da_resize (da_void_t *self, int scale_count);
da_void_t *da_insert (da_void_t *self, int index);

#endif
/* end of file */
