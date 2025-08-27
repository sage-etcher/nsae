
#include "log.h"
#include "ram.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


int
ram_init (ram_t *self, uint8_t prom[], size_t n)
{
    assert (self != NULL);
    assert (prom != NULL);


    /* blank everything */
    (void)memset (self, 0, sizeof (*self));

    /* load prom */
    ram_load_prom (self, prom, n);

    return 0;
}

int
ram_load_prom (ram_t *self, uint8_t prom[], size_t n)
{
    if (n > RAM_PROM_SIZE)
    {
        log_error ("nsae: ram: failed to initialize: prom too large\n");
        return 1;
    }

    /* load prom */
    (void)memcpy (self->m + RAM_BASE_PROM, prom, n);

    return 0;
}

int
ram_load_prom_from_file (ram_t *self, char *file)
{
    FILE *fp = NULL;

    assert (file != NULL);

    fp = fopen (file, "rb");
    if (fp == NULL)
    {
        log_error ("nsae: ram: failed to open prom file -- %s\n", file);
        return 1;
    }

    uint8_t buf[RAM_PROM_SIZE];
    int n = fread (buf, sizeof (uint8_t), RAM_PROM_SIZE, fp);
    fclose (fp);

    if (n != RAM_PROM_SIZE)
    {
        log_warning ("nsae: ram: prom file too small -- %s\n", file);
    }

    ram_load_prom (self, buf, n);

    log_verbose ("nsae: ram: loaded prom file -- %s\n", file);

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
