/*
 * Copyright (c) 2019-2020 Damien Ciabrini
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

// Additional resources for palette
// https://wiki.neogeodev.org/index.php?title=Palettes

#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <string.h>
#include <stdio.h>


/// Start of gradient tiles in BIOS ROM
#define SROM_GRADIENT_TILE_OFFSET 1280


// Color #0 in the palette is always transparent
// so start from #1, and skip the last one
void gradient(int x, int y, int pal) {
    *REG_VRAMADDR=ADDR_FIXMAP+((x+1)<<5)+y;
    *REG_VRAMMOD=32;
    for (u16 i=1; i<16;i++)
        *REG_VRAMRW=(u16)((pal<<12)|(SROM_GRADIENT_TILE_OFFSET+i));
    pal++;
    for (u16 i=1; i<16;i++)
        *REG_VRAMRW=(u16)((pal<<12)|(SROM_GRADIENT_TILE_OFFSET+i));
}

const u16 clut[][16]= {
    // 4 palettes for a 60 colors red gradient
    // 4 palettes for a 60 colors green gradient
    // 4 palettes for a 60 colors blue gradient
    // 4 palettes for a 60 colors white gradient
    #include "gradients.pal"
};

void init_palette() {
    /// Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<256; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
}



int main(void) {
    ng_cls();
    init_palette();

    ng_text(6, 5, 14, "COLOR GRADIENT WITH DARK BIT");
    ng_text(3, 7, 14, "(visible on latest ngdevkit-gngeo)");

    gradient(4, 12, 0);
    gradient(4, 13, 4);
    gradient(4, 14, 8);
    gradient(4, 15, 12);

    gradient(4, 20, 2);
    gradient(4, 21, 6);
    gradient(4, 22, 10);
    gradient(4, 23, 14);

    for(;;) {}
    return 0;
}
