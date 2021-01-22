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

#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/bios-ram.h>
#include <stdio.h>


// This game defines a couple of software DIPs, and use the three
// different sort of DIPs: time DIP, integer DIP, and enum DIP
//
// Each setting has to be defined prior to including the DIP
// helpers API.



/*
 * DEFINING THE SOFTWARE DIP SETTINGS USED BY THIS ROM
 */

// A DIP setting is a macro DIP_<xxx>_OPTS(opt) that define
// a space-separated list opt(<dip_num>, <dip_name>, <options>...)
//

// We use time DIP #0, which defaults to <2>min <30>sec
#define DIP_TIME_OPTS(opt) \
    opt(0,"ROUND TIME", 0x02, 0x30)

// We use two integer DIPs, with their respective default values
#define DIP_INT_OPTS(opt) \
    opt(0,"STAMINA", 20)  \
    opt(1,"RESPAWN", 3)

// We use two enum DIPs. Enum DIP #0 has three possible values
// and defaults to 2 ("PONY") DIP #1 has two values and defaults
// to 1 ("FAST")
#define DIP_ENUM_OPTS(opt)                      \
    opt(0, "ANIMAL", ("CAT","DOG","PONY"), 2)   \
    opt(1, "VARIANT", ("SLOW","FAST"), 1)

// Once the ROM's software DIP are define, include the DIP helpers API
// that will generate the proper information for building the ROM
#include <ngdevkit/soft-dip.h>

// Generate the JP DIP from the generic macros defined above
GENERATE_DIP(jp, "SOFTDIP TEST");
// Same information for the US DIP
GENERATE_DIP(us, "SOFTDIP TEST");
// and for the EU DIP
GENERATE_DIP(eu, "SOFTDIP TEST");



/*
 * READING THE SOFTWARE DIP SETTINGS AT RUNTIME
 */

// On startup, software DIP settings are copied in memory by the
// MVS BIOS. One can either read DIP settings from variable
// `bios_game_dip`, or use helper macros that behave as C arrays.

// the array of time DIPs (2 elements)
#define DIP_TIME ((dip_time_t*)&bios_game_dip[0])
// the array of integer DIPs (2 elements)
#define DIP_INT  ((u8*)&bios_game_dip[4])
// the array of enum DIPs (10 elements)
#define DIP_ENUM ((u8*)&bios_game_dip[6])



void init_palette() {
  const u16 clut[][16]={
      {0x8000, 0x0fff, 0x0533, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
      {0x8000, 0x0fff, 0x026c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
  };
  u16 *p=(u16*)clut;
  for (u16 i=0;i<sizeof(clut)/2; i++) {
      MMAP_PALBANK1[i]=p[i];
  }
}



int main(void) {
  static char tmp[20];

  ng_cls();

  ng_center_text_tall(8, 0, "MVS SOFTWARE DIP TEST");
  ng_center_text(10, 0, "loaded from BRAM at startup");

  ng_text(12, 15, 0, "Round time:");
  ng_text(12, 16, 0, "Stamina...:");
  ng_text(12, 17, 0, "Respawn...:");
  ng_text(12, 18, 0, "Character.:");
  ng_text(12, 19, 0, "Style.....:");

  // Round time
  sprintf(tmp, "%02x:%02x", DIP_TIME[0].min, DIP_TIME[0].sec);
  ng_text(24, 15, 1, tmp);

  // Stamina
  sprintf(tmp, "%d", DIP_INT[0]);
  ng_text(24, 16, 1, tmp);

  // Respawn
  u8 respawn = DIP_INT[1];
  switch (respawn) {
  case 0:
      ng_text(24, 17, 1, "without");
      break;
  case 100:
      ng_text(24, 17, 1, "infinite");
      break;
  default:
      sprintf(tmp, "%d times", respawn);
      ng_text(24, 17, 1, tmp);
  }

  // We must null-terminate enum DIP strings
  tmp[12] = '\0';

  // Character
  u8 animal = DIP_ENUM[0];
  snprintf(tmp, 12, "%s", dip_eu_enum_0_choice[animal]);
  ng_text(24, 18, 1, tmp);

  // Style
  u8 variant = DIP_ENUM[1];
  snprintf(tmp, 12, "%s", dip_eu_enum_1_choice[variant]);
  ng_text(24, 19, 1, tmp);

  for(;;) {}
  return 0;
}

