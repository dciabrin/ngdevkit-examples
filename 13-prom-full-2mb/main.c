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

// Additional resources for memory mapping
// https://wiki.neogeodev.org/index.php?title=68k_memory_map

#include <stdio.h>
#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>

// The 68K program ROM is accessed from two non-contiguous
// memory ranges:
//   [0x000000 .. 0x0fffff] [1MiB] : boot code + main program
//   [0x200000 .. 0x2fffff] [1MiB] : optional additional program space
// Each range is stored in a dedicated P-ROM.
//
// The linker allocates the entire contents of an object file at once.
// If the first range is full, the linker allocates all the remaining
// object file to the second range.
//

// The symbols below are only global variables, so they will be
// allocated in the first P-ROM
u8 bss_byte_ex;
u8 data_byte_ex = 42;
const u16 rodata_short_ex = 1337;

// Define very large functions in another object file to ensure
// that this file's functions will be allocated in the second P-ROM
extern void big_func1(void);
extern void big_func2(void);

// All the functions below will be allocated to the second P-ROM
void big_func3(void) {
    __asm__ (".zero 256*1024");
}
void big_func4(void) {
    __asm__ (".zero 256*1024");
}

int main(void) {
    ng_cls();

    const u16 clut[][16]= {
        { 0x8000, 0x0fff, 0x0555 },
        { 0x8000, 0x0fff, 0x004a },
        { 0x8000, 0x0fff, 0x0a40 }
    };
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }

    ng_center_text_tall(5, 0, "2MB PROGRAM ROM TEST");
    ng_center_text(8, 0, "cartridge using two P-ROMs");

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
    snprintf(str, 40, "big-func3 (0x%06X)", big_func3);
    ng_center_text(25, 2, str);
    snprintf(str, 40, "big-func4 (0x%06X)", big_func4);
    ng_center_text(26, 2, str);
    snprintf(str, 40, "main      (0x%06X)", main);
    ng_center_text(27, 2, str);

    for(;;) {}
    return 0;
}
