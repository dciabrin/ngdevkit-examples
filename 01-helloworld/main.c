/*
 * Copyright (c) 2015-2020 Damien Ciabrini
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

#include <ngdevkit/neogeo.h>

/// Start of character tiles in GAME ROM
#define SROM_TXT_TILE_OFFSET 0

/// Transparent tile in GAME ROM
#define SROM_EMPTY_TILE 255


/// Clear the 40*32 tiles of fix map
void ng_cls() {
    u16 val = SROM_EMPTY_TILE;
    *REG_VRAMADDR = ADDR_FIXMAP;
    *REG_VRAMMOD = 1;
    for (u16 i=1280; i!=0; i--) *REG_VRAMRW = val;
}

/// Handy function to display a string on the fix map
void ng_text(u8 x, u8 y, const char *text) {
    u16 base_val = SROM_TXT_TILE_OFFSET;
    *REG_VRAMADDR=ADDR_FIXMAP+(x<<5)+y;
    *REG_VRAMMOD=32;
    while (*text) *REG_VRAMRW = (u16)(base_val+*text++);
}


int main(void) {
  ng_cls();

  // Set up a minimal palette
  const u16 palette[]={0x8000, 0x0fff, 0x0555};
  for (u16 i=0; i<3; i++) {
    MMAP_PALBANK1[i]=palette[i];
  }

  // Salute the world!
  const char hello1[]="Hello NEO-GEO!";
  const char hello2[]="https://github.com/dciabrin/ngdevkit";
  ng_text((42-sizeof(hello1))/2, 13, hello1);
  ng_text((42-sizeof(hello2))/2, 15, hello2);

  for(;;) {}
  return 0;
}
