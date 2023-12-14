/*
 * Copyright (c) 2020-2023 Damien Ciabrini
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
#include <sys/param.h>
#include <stdio.h>
#include <string.h>


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


#define TOP 10
#define MENUTOP 16
#define MENULEFT 13
#define SAMPLES 8
#define BASE_OFFSET 3
#define LOOP_OFFSET (BASE_OFFSET+SAMPLES)

int main(void) {
    // Command 3: reset z80 sound driver
    *REG_SOUND = 3;
    ng_cls();
    init_palette();

    ng_center_text_tall(6, 0, "ADPCM-B SOUND TEST");
    ng_center_text(TOP, 0, "up/down to select sound command");
    ng_center_text(TOP+1, 1, "button A to play");
    ng_center_text(TOP+2, 2, "button B to loop");

    const char* menu[] = {
        "reset sound",
        "beat normal",
        "beat tempo x0.75",
        "beat tempo x1.5",
        "beat tempo x2",
        "dance normal",
        "dance tempo x0.75",
        "dance tempo x1.5",
        "dance tempo x2"
    };

    s8 selection = 0;

    for(;;) {
        if (bios_p1change & CNT_UP) {
            selection=MAX(0,selection-1);
        } else if (bios_p1change & CNT_DOWN) {
            selection=MIN(selection+1,SAMPLES);
        }

        for (u8 i=0; i<(SAMPLES+1); i++) {
            const char *str=menu[i];
            u8 pal;
            switch (bios_p1current) {
            case A_PRESSED: pal=1; break;
            case B_PRESSED: pal=2; break;
            default: pal=0;
            }
            if (i==selection) {
                ng_text(MENULEFT-2, MENUTOP+i, pal, "<");
                ng_text(MENULEFT, MENUTOP+i, pal, str);
                ng_text(MENULEFT+strlen(str)+1, MENUTOP+i, pal, ">");
            } else {
                ng_text(MENULEFT-2, MENUTOP+i, 0, " ");
                ng_text(MENULEFT, MENUTOP+i, 0, str);
                ng_text(MENULEFT+strlen(str)+1, MENUTOP+i, 0, " ");
            }
        }

        if (bios_p1change & CNT_A) {
            // Trigger the sound command that play a ADPCM-B sample
            *REG_SOUND = BASE_OFFSET + selection;
        } else if (bios_p1change & CNT_B) {
            // The next sound commands play the same ADPCM-B samples,
            // but with sample looping enabled.
            *REG_SOUND = ((selection>0)?LOOP_OFFSET:BASE_OFFSET) + selection;
        }

        ng_wait_vblank();
    }
    return 0;
}
