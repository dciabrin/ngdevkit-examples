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

// Additional resources for bankswitching
// https://wiki.neogeodev.org/index.php?title=Bankswitching

#include <ngdevkit/neogeo.h>
#include <ngdevkit/bios-ram.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <ngdevkit/bank-switch.h>
#include <stdio.h>


// When P-ROM bank switching is used, all the P-ROMs
// share a common ROM base (libc, ngdevkit helpers...),
// and have a specific ROM part.
// In this example we implement an ad-hoc interface
// by setting up a jump table accessible from a
// global variable that is stored in the bank-switched
// address space from P-ROM2.
#include <example_interface.h>
extern const bank_interface_t example_interface;


// This global variable is shared by both banks
u8 common_stage;


const u16 clut[][16]= {
    { 0x8000, 0x0fff, 0x0555 },
    { 0x8000, 0x0fff, 0x004a },
    { 0x8000, 0x0fff, 0x0a40 }
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
    ng_cls();
    init_palette();

    ng_center_text_tall(4, 0, "P-ROM BANK SWITCHING TEST");
    ng_center_text(20, 0, "Use buttons to update variables");
    ng_center_text(21, 0, "Use UP/DOWN to switch P-ROM banks");

    example_interface.init_bank_on_switch();

    for(;;) {
        // This example is compiled as 'pulstar'. This ROM has a second
        // P-ROM of size 2MB, which means two banks of 1MB. At runtime,
        // a single bank of 1MB is mapped at a time into the memory space.
        u8 bank = 0;
        if ((bios_p1change & CNT_UP) && (bios_p1current & CNT_UP)) {
            bank = 2;
        } else if ((bios_p1change & CNT_DOWN) && (bios_p1current & CNT_DOWN)) {
            bank = 1;
        }

        // Write the bank number into memory to trigger the bank switch
        if (bank) {
            P_ROM_SWITCH_BANK(bank-1);
            example_interface.init_bank_on_switch();
        }

        // Call the bank's interface functions
        example_interface.read_inputs();
        example_interface.print_various_globals();

        ng_wait_vblank();
    }
    return 0;
}
