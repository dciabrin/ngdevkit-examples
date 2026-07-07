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

# This example uses a memory card; MEMCARD below creates an empty card
# image and passes it to the emulators that support it.
ngdevkit_elf(18-memory-card-rom
  SOURCES      main.c
  CFLAGS       -fomit-frame-pointer -O2 -g
  LINK_OPTIONS -Wl,--defsym,rom_NGH_ID=1338 -Wl,--defsym,rom_eye_catcher_mode=0
)

ngdevkit_cartridge(
  GAMEROM 18_memory_card
  TITLE   "Accessing the memory card"
  ELF     18-memory-card-rom
  MEMCARD
)
