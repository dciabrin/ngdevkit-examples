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

#ifndef __EXAMPLE_INTERFACE_H__
#define __EXAMPLE_INTERFACE_H__

// When P-ROM bank switching is used, all the P-ROMs
// share a common ROM base (libc, ngdevkit helpers...),
// and have a specific ROM part.

// You can call a function which is implemented
// differently in each P-ROM, by setting up a jump table
// as a sort of ad-hoc interface, as shown below.

typedef struct {
    // Clear all global variables specific to the bank
    // Note: all the globals variables from the banks
    // are not initialized by the C runtime. You have
    // to explicitely clear them / initialize them
    // any time you switch to a specific bank.
    void (*init_bank_on_switch)(void);
    // This example function reads joystick inputs and
    // updates global variables, including those specific
    // to the bank

    void (*read_inputs)(void);

    // This example function prints common and bank-specific
    // global variables
    void (*print_various_globals)(void);
} bank_interface_t;

#endif /* __EXAMPLE_INTERFACE_H__ */
