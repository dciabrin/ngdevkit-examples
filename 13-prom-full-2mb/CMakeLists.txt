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

# The program overflows the first 1MB P-ROM; the linker places the
# overflowing code into section .text2, which is packed into a second
# P-ROM built from the same ELF file.
ngdevkit_elf(13-prom-full-2mb-rom
  SOURCES      main.c overflow.c
  LINK_OPTIONS -Wl,--enable-non-contiguous-regions
)

ngdevkit_cartridge(
  GAMEROM   13_prom_2mb
  TITLE     "P-ROM 2MB non-contiguous"
  ELF       13-prom-full-2mb-rom
  PROM2_ELF 13-prom-full-2mb-rom
)
