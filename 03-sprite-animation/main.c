/*
 * Copyright (c) 2018-2020 Damien Ciabrini
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

#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>


const u16 clut[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fa0, 0x0644, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // inline sprite's palette (C format)
    #include "sprite.pal"
};

void init_palette() {
    /// Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<32; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    *((volatile u16*)0x401ffe)=0xa80;
}

char str[16];

s16 x=130;
s16 y=-80;

// Sprite setup.
//
// The example sprite is 4 x 4 tiles
// The animation uses 8 different frames/sprites
void update_sprite(u16 start_tile, u16 mirror) {
    static const u8 right_tiles[]={0,1,2,3};
    static const u8 left_tiles[]={3,2,1,0};
    const u8 *tiles = mirror?left_tiles:right_tiles;
    *REG_VRAMMOD=1;
    for (u8 i=0; i<4; i++) {
        *REG_VRAMADDR=ADDR_SCB1+(i*64); // i-th sprite in SCB1
        u16 tile = start_tile + tiles[i];
        u16 attr = (1<<8) | mirror;
        *REG_VRAMRW=tile;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+4;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+8;
        *REG_VRAMRW=attr;
        *REG_VRAMRW=tile+12;
        *REG_VRAMRW=attr;
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom, 4 tiles tall
    *REG_VRAMADDR=ADDR_SCB2;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(y<<7)+4;
    *REG_VRAMRW=(x<<7);
    for (u16 i=1; i<4; i++) {
        *REG_VRAMADDR=ADDR_SCB2+i;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6;
    }
}


// Get joystick status and move sprite position
void check_move_sprite()
{
    static char joystate[5]={'0','0','0','0',0};

    u8 u=(bios_p1current & CNT_UP);
    u8 d=(bios_p1current & CNT_DOWN);
    u8 l=(bios_p1current & CNT_LEFT);
    u8 r=(bios_p1current & CNT_RIGHT);

    joystate[0]=u?'1':'0';
    joystate[1]=d?'1':'0';
    joystate[2]=l?'1':'0';
    joystate[3]=r?'1':'0';

    if (u) {y+=1;}
    if (d) {y-=1;}
    if (l) {x-=1;}
    if (r) {x+=1;}

    snprintf(str, 15, "JS1 %s", joystate);
    ng_text(2, 25, 0, str);
}


static u8 frame_cycle;
static u8 vbl=4;

void set_player_state()
{
    // tile positions in cROM for the animations
    // each frame is made of 16 tiles
    static const u16 idle_frame[8] = {256, 272, 288, 304, 320, 336, 352, 368};
    static const u16 walk_frame[8] = {384, 400, 416, 432, 448, 464, 480, 496};
    if (vbl-- == 0) {
        frame_cycle = (frame_cycle+1) & 7;
        vbl = 4;
    }

    u8 l = (bios_p1current & CNT_LEFT);
    u8 r = (bios_p1current & CNT_RIGHT);

    static u8 mirror;
    const u16* frame;
    if (!l && !r) {
        frame = idle_frame;
    } else {
        frame = walk_frame;
        mirror = l?1:0;
    }
    update_sprite(frame[frame_cycle], mirror);

    snprintf(str, 15, "frame  %d", frame_cycle);
    ng_text(2, 26, 0, str);
    snprintf(str, 15, "mirror %d", mirror);
    ng_text(2, 27, 0, str);
    snprintf(str, 15, "sprite %s", frame == idle_frame?"idle":"walk");
    ng_text(2, 28, 0, str);
}


int main(void) {
    ng_cls();
    init_palette();

    ng_text_tall(24, 27, 0, "ANIMATION TEST");
    ng_text(4, 18, 0, "Move the sprite with the joystick!");

    for(;;) {
        set_player_state();
        check_move_sprite();
        ng_wait_vblank();
    }
    return 0;
}
