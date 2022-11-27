/*
 * Copyright (c) 2022 Damien Ciabrini
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

// Note: this file's name has substring '__bank' in it, so the
// linker automatically allocates all its symbols to the address
// space of P-ROM2 (0x200000..0x2FFFFF).


#include <stdio.h>
#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/bios-ram.h>


// These functions implement the example interface above
void init_bank_on_switch(void);
void read_inputs(void);
void print_various_globals(void);


// The first symbol of all banks must be the interface
// so that the address doesn't change after bank switch.
#include "example_interface.h"
const bank_interface_t example_interface = {
    init_bank_on_switch,
    read_inputs,
    print_various_globals
};

// Other globals must be uninitialized, as globals
// that are bank-specific aren't managed by the C
// runtime, and because these globals don't survive
// between bank switches.
u8 bank0_lives;
u8 bank0_velocity;

// This global lies in the common part of the P-ROMs
extern u8 common_stage;


void init_bank_on_switch(void)
{
    ng_center_text(10, 1, "<bank 0 running>");

    for (u8 i=13; i<16; i++) {
        ng_text(6, i, 1, "                            ");
    }
    ng_text(8, 13, 1, "[A] common - stage:");
    ng_text(8, 14, 1, "[B] bank 0 - lives:");
    ng_text(8, 15, 1, "[C] bank 0 - velocity:");

    bank0_lives = 0;
    bank0_velocity = 0;
}

void read_inputs(void)
{
    if ((bios_p1change & CNT_A) && (bios_p1current & CNT_A)) {
        common_stage++;
    }
    if ((bios_p1change & CNT_B) && (bios_p1current & CNT_B)) {
        bank0_lives++;
    }
    if ((bios_p1change & CNT_C) && (bios_p1current & CNT_C)) {
        bank0_velocity++;
    }
}

void print_various_globals(void)
{
    char str[20];
    sprintf(str, "%02x", common_stage);
    ng_text(31, 13, 1, str);
    sprintf(str, "%02x", bank0_lives);
    ng_text(31, 14, 1, str);
    sprintf(str, "%02x", bank0_velocity);
    ng_text(31, 15, 1, str);
}
