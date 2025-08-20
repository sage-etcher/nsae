
#include "ram.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


int
ram_init (ram_t *self, uint8_t prom[], size_t n)
{
    assert (self != NULL);
    assert (prom != NULL);

    if (n >= RAM_PROM_SIZE)
    {
        return 1;
    }

    /* blank everything */
    (void)memset (self, 0, sizeof (*self));

    /* load prom */
    (void)memcpy (self->m + RAM_BASE_PROM, prom, n);

    return 0;
}

uint8_t
ram_read (ram_t *self, uint32_t addr)
{
    assert (self != NULL);
    assert (addr < RAM_SIZE);

    return self->m[addr];
}

int
ram_write (ram_t *self, uint32_t addr, uint8_t data)
{
    assert (self != NULL);
    assert (addr < RAM_SIZE);

    if (addr >= RAM_BASE_PROM)
    {
        return 1;
    }

    self->m[addr] = data;

    return 0;
}


/* end of file */
