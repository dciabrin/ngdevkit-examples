/*
 * Copyright (c) 2021 Damien Ciabrini
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

// Additional resources for sprites
// https://wiki.neogeodev.org/index.php?title=Sprites
// https://wiki.neogeodev.org/index.php?title=Timer_interrupt
// https://wiki.neogeodev.org/index.php?title=Display_timing#Vertical
// https://wiki.neogeodev.org/index.php?title=Memory_mapped_registers#REG_LSPCMODE

#include <ngdevkit/neogeo.h>
#include <ngdevkit/registers.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>



// A sine-like precalc buffer
static u16 wobble[] = {
#include "precalc.inc"
};


// Sprite data
#define START_TILE 256

// Sprite coordinates
s16 x=40;
s16 y=-80;

// Offset for top of sprite. updated once per frame
u8 frame_offset = 0;
// Row offset. updated once per line
u8 offset = 0;



// Sprite setup.
// Look at example 02-sprite for details
void init_sprite() {
    *REG_VRAMMOD=1;

    for (u8 i=0; i<15; i++) {
        *REG_VRAMADDR=ADDR_SCB1+(i*64);
        u16 tile = START_TILE + i;
        u16 attr = 1<<8;
        *REG_VRAMRW=tile;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+15;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+30;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+45;
        *REG_VRAMRW=attr;
    }
    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(y<<7)+4;
    *REG_VRAMRW=(wobble[frame_offset]<<7);

    for (u16 i=1; i<15; i++) {
        *REG_VRAMADDR=ADDR_SCB2+i;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6;
    }
}


// At each line, the effective sprite's x position is
// updated by an offset, taken from the precalc
void rom_callback_Timer() {
    offset=(offset+1)&63;

    /* Update sprite X position */
    u16 o = (frame_offset+offset)&63;
    *REG_VRAMADDR = ADDR_SCB3+0x200;
    *REG_VRAMRW = (wobble[o]<<7);
}


void init_palette() {
    const u16 clut[]= { 0x8000, 0x0fa0, 0x0644, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                        0x0000, 0x0fff, 0x0ddd, 0x0aaa, 0x7555, 0x306E, 0x0000, 0x0000,
                        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
    for (u16 i=0;i<32; i++) {
        MMAP_PALBANK1[i] = clut[i];
    }
}


int main(void) {
    ng_cls();
    init_palette();
    init_sprite();

    ng_center_text_tall(22, 0, "TIMER INTERRUPT TEST");
    ng_center_text(25, 0, "(visible on latest ngdevkit-gngeo)");

    ng_wait_vblank();

    // configure the timer interrupt to fire after 384 pixels, which
    // represent a full line worth of pixels:
    //   . 28 invisible pixels (hsync)
    //   . 28 invisible pixels (back porch)
    //   . 320 visible pixels (actual screen)
    //   . 8 invisible pixels (front porch)
    *REG_TIMERHIGH = 0;
    *REG_TIMERLOW = 384;

    // Enable Timer interrupt, and rearm when vsync is reached
    // or timer counter reaches 0
    *REG_LSPCMODE = 0xd<<4;

    for(;;) {
        ng_wait_vblank();
        offset = 0;
        frame_offset = (frame_offset+1)&63;
    }
    return 0;
}
