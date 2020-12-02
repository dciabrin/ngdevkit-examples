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
#include <stdio.h>

/// for snprintf()
int __errno;
char str[30];

// Address of Sprite Control Block in VRAM
#define ADDR_SCB1      0
#define ADDR_SCB2 0x8000
#define ADDR_SCB3 0x8200
#define ADDR_SCB4 0x8400

/// Transparent tile in BIOS ROM
#define SROM_EMPTY_TILE 255

/// Start of character tiles in BIOS ROM
#define SROM_TXT_TILE_OFFSET 0

/// z80 sound driver communication
#define REG_SOUND 0x320000


/// Handy function to display a string on the fix map
void display(int x, int y, const char *text) {
  *REG_VRAMADDR=ADDR_FIXMAP+(x<<5)+y;
  *REG_VRAMMOD=32;
  while (*text) *REG_VRAMRW=(u16)(SROM_TXT_TILE_OFFSET+*text++);
}

// Clear the 40*32 tiles of fix map
void clear_tiles() {
    *REG_VRAMADDR=ADDR_FIXMAP;
    *REG_VRAMMOD=1;
    for (u16 i=0;i<40*32;i++) {
        *REG_VRAMRW=(u16)SROM_EMPTY_TILE;
    }
}

const u16 clut[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
};

void init_palette() {
    /// Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    *((volatile u16*)0x401ffe)=0x0333;
}


// Vertical blanking.
/* volatile u8 vblank=0; */

/* void rom_callback_VBlank() { */
/*     vblank=1; */
/* } */

/* void wait_vblank() { */
/*     while (!vblank); */
/*     vblank=0; */
/* } */

u8 button_a=0xff;
u8 button_b=0xff;
u8 button_c=0xff;

u8 update_buttons()
{
    // A button pressed
    u8 prev_button_a = button_a;
    u8 prev_button_b = button_b;
    u8 prev_button_c = button_c;
    button_a = *REG_P1CNT&0x10;
    button_b = *REG_P1CNT&0x20;
    button_c = *REG_P1CNT&0x40;

    u8 pressed_a = ((prev_button_a != button_a) && (button_a == 0))?1:0;
    u8 pressed_b = ((prev_button_b != button_b) && (button_b == 0))?2:0;
    u8 pressed_c = ((prev_button_c != button_c) && (button_c == 0))?4:0;
    return pressed_c | pressed_b | pressed_a;
}

int main(void) {
    *((volatile u8*)REG_SOUND)=0x3;
    clear_tiles();
    init_palette();

#define TOP 2
    display(9, TOP+6, "PRESS BUTTONS TO PLAY");
    display(9, TOP+7, "   ADPCM-A SAMPLES   ");
    display(6, TOP+11, "CHANNEL 1         CHANNEL 2");
    display(6, TOP+13, " play on         play until");
    display(6, TOP+14, " pressed          finished");
    display(6, TOP+19, "A woosh:         C break: ");
    display(6, TOP+21, "B  kick:");

    for(;;) {
        u8 pressed = update_buttons();
        display(15, TOP+19, button_a?"0":"1");
        display(15, TOP+21, button_b?"0":"1");
        display(32, TOP+19, button_c?"0":"1");

        // trigger cmds in in the sound driver, based on input
        if (pressed&1) {
            *((volatile u8*)REG_SOUND)=0x4;
        }
        if (pressed&2) {
            *((volatile u8*)REG_SOUND)=0x5;
        }
        if (pressed&4) {
            *((volatile u8*)REG_SOUND)=0x6;
        }
        /* wait_vblank(); */
    }
    return 0;
}
