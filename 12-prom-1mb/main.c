/*
 * Copyright (c) 2022 Damien Ciabrini
 * This file is part of ngdevkit
 *
 * ngdevkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ngdevkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>


// The previous example in ngdevkit all generate a 512KB P-ROM.
// Here we define very large functions to fill the 1MiB address
// space in the P-ROM.
// Note: To load properly in the emulator, this program must be
// compiled to a ROM name which is known to use 1MB of P-ROM.
// In the makefile, ssideki2 is used.

void big_func1(void) {
    __asm__ (".zero 256*1024");
}

void big_func2(void) {
    __asm__ (".zero 256*1024");
}

// The symbols below are only global variables, they will be
// allocated first in the P-ROM
u8 bss_byte_ex;
u8 data_byte_ex = 42;
const u16 rodata_short_ex = 1337;

int main(void) {
    ng_cls();

    const u16 clut[][16]= {
        { 0x8000, 0x0fff, 0x0555 },
        { 0x8000, 0x0fff, 0x004a }
    };
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }

    ng_center_text_tall(5, 0, "1MB PROGRAM ROM TEST");

    char str[40];
    ng_center_text(13, 0, "global variables (start of P-ROM)");
    snprintf(str, 40, "bss-byte-ex     (0x%06X) : %d", &bss_byte_ex, bss_byte_ex);
    ng_text(4, 15, 1, str);
    snprintf(str, 40, "data-byte-ex    (0x%06X) : %d", &data_byte_ex, data_byte_ex);
    ng_text(4, 16, 1, str);
    snprintf(str, 40, "rodata-short-ex (0x%06X) : %d", &rodata_short_ex, rodata_short_ex);
    ng_text(4, 17, 1, str);

    ng_center_text(21, 0, "functions (end of P-ROM)");
    snprintf(str, 40, "big-func1 (0x%06X)", big_func1);
    ng_center_text(23, 1, str);
    snprintf(str, 40, "big-func2 (0x%06X)", big_func2);
    ng_center_text(24, 1, str);
    snprintf(str, 40, "main      (0x%06X)", main);
    ng_center_text(25, 1, str);

    for(;;) {}
    return 0;
}
