
#include "darray.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


da_void_t *
da_resize (da_void_t *self, int scale_count)
{
    size_t new_count = 0;
    size_t new_alloc = 0;
    size_t new_size  = 0;
    void *tmp = NULL;

    assert (self != NULL);
    assert (self->elem_size != 0);

    new_count = self->count + scale_count;
    while (new_count >= self->alloc)
    {
        new_alloc = ((self->alloc == 0) ? 1 : (self->alloc * 2));
        new_size = self->elem_size * new_alloc;
        tmp = realloc (self->m, new_size);
        if (tmp == NULL)
        {
            free (self->m);
            self->m = NULL;
            self->alloc = 0;
            self->count = 0;
            return NULL;
        }

        self->m = tmp;
        self->alloc = new_alloc;
    }

    return self;
}

da_void_t *
da_pop_front (da_void_t *self, int n)
{
    size_t new_count = 0;
    size_t size = 0;

    assert (self != NULL);
    assert (n >= 0);

    if (n == 0) { return self; }

    if ((size_t)n >= self->count)
    {
        self->count = 0;
        return self;
    }

    new_count = self->count - n;
    size = self->elem_size * new_count;

    memcpy (self->m, (uint8_t *)self->m + (n * self->elem_size), size);
    self->count = new_count;

    return self;
}


da_void_t *
da_insert (da_void_t *self, int index)
{
    uintptr_t dst = 0;
    uintptr_t src = 0;
    size_t n = 0;

    assert (self != NULL);
    assert (self->elem_size != 0);
    assert (index >= 0);
    assert ((size_t)index <= self->count);

    da_resize (self, 1);
    self->count++;

    src = (uintptr_t)self->m + (self->elem_size * index);
    dst = src + self->elem_size;
    n = (self->count - index) * self->elem_size;

    memmove ((void *)dst, (void *)src, n);

    return self; 
}


/* end of file */
