/*
 * Copyright (c) 2020 Damien Ciabrini
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

/// Start of sprite tiles
#define CROM_BACKGROUND_OFFSET 256
#define CROM_FOREST_OFFSET 472
#define CROM_FRONT_OFFSET 742

// Fix point logic for slow scrolling
#define FIX_FRACTIONAL_BITS 3
#define FIX_POINT(a,f) ((a<<FIX_FRACTIONAL_BITS)+f)
#define FIX_ACCUM(x) (x>>FIX_FRACTIONAL_BITS)
#define FIX_ZERO (FIX_POINT(1,0)-1)


// Plane and scrolling state
typedef struct _plane_t {
    u16 sprite;
    u16 tile_offset;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 leftmost;
    s16 offset;
    u16 increment;
} plane_t;


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
    /// sprite palettes
    #include "sprites/back.pal"
    #include "sprites/forest.pal"
    #include "sprites/front.pal"
};

void init_palette() {
    /// Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    *((volatile u16*)0x401ffe)=0x18bf;
}


void setup_plane(plane_t *plane) {
    for (u16 s=0; s<plane->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((plane->sprite+s)*64);
        u16 tile = plane->tile_offset+s;
        for (u16 v=0; v<plane->height; v++, tile+=plane->width) {
            *REG_VRAMRW = tile;
            *REG_VRAMRW = (plane->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom, 4 tiles tall
    *REG_VRAMADDR=ADDR_SCB2+plane->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(plane->y<<7)+plane->height;
    *REG_VRAMRW=(plane->x<<7);
    for (u16 v=1, u=plane->x+16; v<plane->width; v++, u+=16) {
        *REG_VRAMADDR=ADDR_SCB2+plane->sprite+v;
        *REG_VRAMRW=0xFFF;
        // not sticky, so explicitely set x,y pos
        *REG_VRAMRW=(plane->y<<7)+plane->height;
        *REG_VRAMRW=(u<<7);
    }
}

void update_plane(plane_t *plane) {
    // A plane is composed of n consecutive vertical sprites,
    // cycling position in a rolling fashion:
    //   . each time the leftmost sprite scrolled 16 pixels left,
    //     it goes to the rightmost position.
    //   . the next sprite in the list becomes the new leftmost one.
    // Effectively, the plane can be seen as two lists of sprites
    // [0..leftmost[ and [leftmost..n[
    // Loop over those two lists to scroll the sprites' x pos.
    s16 x;
    u16 i;
    s16 pixel_diff=FIX_ACCUM(plane->offset)-FIX_ACCUM(plane->offset-plane->increment);
    *REG_VRAMMOD=1;
    *REG_VRAMADDR=ADDR_SCB4+plane->sprite;
    for (i=plane->sprite; i<plane->leftmost;i++) {
        x=*REG_VRAMRW>>7;
        *REG_VRAMRW=((x-pixel_diff)<<7);
    }
    i=plane->leftmost;
    *REG_VRAMADDR=ADDR_SCB4+i;
    x=*REG_VRAMRW>>7;
    *REG_VRAMADDR=ADDR_SCB4+i;
    plane->offset-=plane->increment;
    if (plane->offset<=FIX_ZERO) {
        u16 oldx=x;
        x+=16*plane->width;
        plane->offset+=FIX_POINT(16,0);
        plane->leftmost++;
        if (plane->leftmost>=plane->sprite+plane->width)
            plane->leftmost=plane->sprite;
    }
    *REG_VRAMRW=((x-pixel_diff)<<7);
    *REG_VRAMADDR=ADDR_SCB4+i+1;
    for (i=i+1; i<plane->sprite+plane->width;i++) {
        x=*REG_VRAMRW>>7;
        *REG_VRAMRW=((x-pixel_diff)<<7);
    }
    
}

// Vertical blanking.
volatile u8 vblank=0;

void rom_callback_VBlank() {
    vblank=1;
}

void wait_vblank() {
    while (!vblank);
    vblank=0;
}


// three planes in this example
plane_t back = {
    .sprite = 1,
    .tile_offset = CROM_BACKGROUND_OFFSET,
    .palette = 1,
    .width = 24,
    .height = 7,
    .x = 0,
    .y = -32,
    .leftmost = 1,
    .offset = FIX_POINT(16,0),
    .increment = FIX_POINT(0,1) // 0.125
};

plane_t forest = {
    .sprite = 25,
    .tile_offset = CROM_FOREST_OFFSET,
    .palette = 2,
    .width = 30,
    .height = 9,
    .x = 0,
    .y = -96,
    .leftmost = 25,
    .offset = FIX_POINT(16,0),
    .increment = FIX_POINT(0,4) // 0.5
};

plane_t front = {
    .sprite = 55,
    .tile_offset = CROM_FRONT_OFFSET,
    .palette = 3,
    .width = 24,
    .height = 9,
    .x = 0,
    .y = -96,
    .leftmost = 55,
    .offset = FIX_POINT(16,0),
    .increment = FIX_POINT(1,0) // 1.0
};


int main(void) {
    clear_tiles();
    init_palette();
    setup_plane(&back);
    setup_plane(&forest);
    setup_plane(&front);

    display(2, 3, "Parallax scrolling");

    for(;;) { 
        wait_vblank();
        update_plane(&back);
        update_plane(&forest);
        update_plane(&front);
    }
    return 0;
}
