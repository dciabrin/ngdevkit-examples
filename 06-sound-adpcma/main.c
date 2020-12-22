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
#include <stdio.h>


/// controller's current state, and change on button press (not release)
extern u8 bios_p1current;
extern u8 bios_p1change;
#define A_PRESSED 0x10
#define B_PRESSED 0x20
#define C_PRESSED 0x40


/// z80 sound driver communication
#define REG_SOUND ((u8*)0x320000)

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
    // Command 3: reset z80 sound driver
    *REG_SOUND = 3;
    ng_cls();
    init_palette();

#define TOP 2
    ng_center_text(TOP+6, 0, "PRESS BUTTONS TO PLAY");
    ng_center_text(TOP+7, 0, "   ADPCM-A SAMPLES   ");

    ng_text_tall(6, TOP+11, 1, "CHANNEL 1");
    ng_text(6, TOP+13, 0, " play on");
    ng_text(6, TOP+14, 0, " pressed");
    ng_text(6, TOP+19, 1, "A woosh:");
    ng_text(6, TOP+21, 1, "B  kick:");

    ng_text_tall(24, TOP+11, 2, "CHANNEL 2");
    ng_text(23, TOP+13, 0, "play until");
    ng_text(24, TOP+14, 0, "finished");
    ng_text(23, TOP+19, 2, "C break: ");

    for(;;) {
        ng_text(15, TOP+19, 1, bios_p1current&A_PRESSED?"1":"0");
        ng_text(15, TOP+21, 1, bios_p1current&B_PRESSED?"1":"0");
        ng_text(32, TOP+19, 2, bios_p1current&C_PRESSED?"1":"0");

        // trigger cmds in in the sound driver, based on input
        if (bios_p1change & A_PRESSED) {
            // Command 4: play "woosh" sample
            *REG_SOUND=4;
        }
        if (bios_p1change & B_PRESSED) {
            // Command 5: play "kick" sample
            *REG_SOUND=5;
        }
        if (bios_p1change & C_PRESSED) {
            // Command 6: play "break" sample (uninterruptible)
            *REG_SOUND=6;
        }
    }
    return 0;
}
