/*
 * Copyright (c) 2020 Damien Ciabrini
 * This file is part of ngdevkit-examples
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

// Additional resources for sound
// https://wiki.neogeodev.org/index.php?title=YM2610_registers

#include <ngdevkit/neogeo.h>
#include <ngdevkit/bios-ram.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>


/// controller's current state, and change on button press (not release)
extern u8 bios_p1current;
extern u8 bios_p1change;
#define A_PRESSED 0x10
#define B_PRESSED 0x20
#define C_PRESSED 0x40


/// z80 sound driver communication
#define REG_SOUND ((volatile u8*)0x320000)

const u16 clut[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x8000, 0x0fff, 0x0a40, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x8000, 0x0fff, 0x004a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
};

void init_palette() {
    /// Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    *((volatile u16*)0x401ffe)=0x0333;
}


int main(void) {
    u8 cmd=4;
    char strcmd[3];

    u8 inc = 1;

    // Command 3: reset z80 sound driver
    *REG_SOUND = 3;
    ng_cls();
    init_palette();

#define TOP 2
    ng_center_text_tall(TOP+6, 0, "Z80 DRIVER SOUND TEST");
    ng_center_text(TOP+10, 0, "left/right to select digit");
    ng_center_text(TOP+11, 0, "up/down to change command");
    ng_center_text(TOP+13, 1, "button A for music");
    ng_center_text(TOP+14, 2, "button B for sfx ");
    ng_text(13, TOP+18, 1, " command:");

    for(;;) {
        if (bios_p1change & CNT_LEFT) {
            inc=0x10;
        } else if (bios_p1change & CNT_RIGHT) {
            inc=1;
        }

        if (bios_p1change & CNT_UP) {
            cmd+=inc;
        } else if (bios_p1change & CNT_DOWN) {
            cmd-=inc;
        }

        sprintf(strcmd, "%02X", cmd);
        ng_text(23, TOP+18, 2, strcmd);

        ng_text(23, TOP+17, 0, (inc==1)?" -":"- ");
        ng_text(23, TOP+19, 0, (inc==1)?" -":"- ");

        ng_text(22, TOP+18, 0, bios_p1current&A_PRESSED?"<":" ");
        ng_text(25, TOP+18, 0, bios_p1current&A_PRESSED?">":" ");
        if (bios_p1change & CNT_A) {
            *REG_SOUND=cmd;
        }
        if (bios_p1change & CNT_B) {
            // cmd 7 is laser sound FX in this example VROM
            *REG_SOUND=7;
        }

        ng_wait_vblank();
    }
    return 0;
}
