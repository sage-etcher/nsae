
#include "nsae.h"

#include "mmu.h"
#include "z80emu.h"

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

int
adv_init (adv_t *self)
{
    /* initialize the cpu */
    Z80Reset (&self->cpu);

    /* initialize the mmu */
    self->mmu.slot_bit = 14; /* 16kb slots */

    mmu_init_page (&self->mmu, 0x00, 0xffff, 0x00000); /* 16k main */
    mmu_init_page (&self->mmu, 0x01, 0xffff, 0x04000); /* 16k main */
    mmu_init_page (&self->mmu, 0x02, 0xffff, 0x08000); /* 16k main */
    mmu_init_page (&self->mmu, 0x03, 0xffff, 0x0C000); /* 16k main */
    mmu_init_page (&self->mmu, 0x08, 0xffff, 0x10000); /* 16k video */
    mmu_init_page (&self->mmu, 0x09, 0x0fff, 0x14000); /*  4k video */
    mmu_init_page (&self->mmu, 0x0C, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0D, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0E, 0x07ff, 0x15000); /*  2k prom */
    mmu_init_page (&self->mmu, 0x0F, 0x07ff, 0x15000); /*  2k prom */

    mmu_load_page (&self->mmu, 0x00, 0x08); /* video */
    mmu_load_page (&self->mmu, 0x01, 0x09); /* video */
    mmu_load_page (&self->mmu, 0x02, 0x0C); /* prom */
    mmu_load_page (&self->mmu, 0x03, 0x00); /* main */


    /* initialize memory */
    adv_write (self, 0x0000, 0xC3); /* jmp main */
    adv_write (self, 0x0001, 0x00);
    adv_write (self, 0x0002, 0xc0);

    uint8_t buf[] = {
        0xaf,               /* main:   xra     a        */
        0x32, 0x00, 0x00,   /*         sta     0x0000   */
        0x32, 0x01, 0x00,   /*         sta     0x0001   */
        0x32, 0x02, 0x00,   /*         sta     0x0002   */
        0x21, 0x00, 0x00,   /*         lxi     h,0x0000 */
        0x06, 0xf0,         /* loop:   mvi     b,0xf0   */
        0x7e,               /*         mov     a,m      */
        0xa8,               /*         xra     b        */
        0x77,               /*         mov     m,a      */
        0x24,               /*         inr     h        */
        0x7c,               /*         mov     a,h      */
        0xfe, 0x50,         /*         cpi     80       */
        0xda, 0x0d, 0xc0,   /*         jc      loop     */
        0x26, 0x00,         /*         mvi     h,0      */
        0x2c,               /*         inr     l        */
        0x7d,               /*         mov     a,l      */
        0xfe, 0xF0,         /*         cpi     240      */
        0xda, 0x0d, 0xc0,   /*         jc      loop     */
        0x2e, 0x00,         /*         mvi     l,0      */
        0x11, 0x01, 0x00,   /* wait:   lxi     d,1      */
        0x11, 0x01, 0x00,   /*         lxi     d,1      */
        0x19,               /*         dad     d        */
        0xd2, 0x24, 0xc0,   /*         jnc     wait     */
        0xc3, 0x0d, 0xc0,   /*         jmp     loop     */
    };

    for (size_t i = 0; i < sizeof (buf); i++)
    {
        adv_write (self, 0x0c000 + i, buf[i]);
    }


    uint8_t buf_letter[] = {
        0b00000000, 0b11000000, 0b00000000,
        0b00000001, 0b01100000, 0b00000000,
        0b00000011, 0b00110000, 0b00000000,
        0b00000110, 0b00011000, 0b00000000,
        0b00001100, 0b00001100, 0b00000000,
        0b00011111, 0b11111110, 0b00000000,
        0b00110000, 0b00000011, 0b00000000,
        0b01100000, 0b00000001, 0b10000000,
        0b11110000, 0b00000011, 0b11000000,
    };
    size_t letter_x = 1;
    size_t letter_y = 4;
    size_t letter_w = 3;
    size_t letter_h = 9;

    for (size_t x = 0; x < letter_w; x++)
    {
        for (size_t y = 0; y < letter_h; y++)
        {
            uint32_t offset = (((x + letter_x) << 8) | (y + letter_y));
            adv_write (self, offset, buf_letter[y*letter_w+x]);
        }
    }

    return 0;
}

int
adv_run (adv_t *self, int number_cycles)
{
    assert (self != NULL);

    return Z80Emulate (&self->cpu, number_cycles, self);
}

uint8_t
adv_read (adv_t *self, uint16_t addr)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    return self->memory[abs_addr];
}

void
adv_write (adv_t *self, uint16_t addr, uint8_t data)
{
    assert (self != NULL);

    uint32_t abs_addr = mmu_decode (&self->mmu, addr);
    assert (abs_addr < ADV_RAM);

    /* dont write to ROM */
    if (abs_addr >= 0x15000) 
    {
        return;
    }

    self->memory[abs_addr] = data;
}

uint8_t
adv_in (adv_t *self, uint8_t port)
{
    return 0x00;
}

void
adv_out (adv_t *self, uint8_t port, uint8_t data)
{
    return;
}

/* end of file */
