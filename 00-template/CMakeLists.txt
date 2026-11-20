# Copyright (c) 2026 Florian Loitsch
# This file is part of ngdevkit
#
# ngdevkit is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# ngdevkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.

# This is the CMake counterpart of the template's Makefile. The helper
# functions used below are defined in cmake/NGDevKit.cmake.

# program ROM: your main program.
# List your sources below; they are compiled into an ELF binary that
# becomes the content of the program ROM (PROM1).
# Default compile flags can be replaced per target with CFLAGS, and
# linker flags added with LINK_OPTIONS.
ngdevkit_elf(00-template-rom
  SOURCES main.c
)

# The cartridge: all the ROM chips, the cartridge zip, the emulator
# hash files, plus run targets (00-template-gngeo, 00-template-mame, ...).
#
# By default:
#   . the fixed tiles ROM (SROM) holds latin fonts for printing text
#   . the sprite ROM (CROM1/CROM2) holds the ngdevkit logo shown
#     during the attract mode
#   . the sound driver ROM (MROM) holds ngdevkit's base sound driver
#   . the sample ROM (VROM) is empty
# Override them with the SROM, CROM1/CROM2, SOUND_DRIVER and VROM
# arguments; see the other examples for how to convert your own assets.
ngdevkit_cartridge(
  GAMEROM ngdevkit
  TITLE   "ngdevkit template ROM"
  ELF     00-template-rom
)
