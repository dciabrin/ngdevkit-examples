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

# Bankswitching program ROM is achieved by building several ELF files
#  - The first ELF file contains the common code that is not bank-switched,
#    it gets built and converted into a regular P-ROM
#  - For every bank, a dedicated ELF file is built that contains only
#    code that belongs to the bank. The ngdevkit linkscript achieves that
#    by automatically linking all files whose path or name contains '__bank'
#    into the bank-switch P-ROM address space
# The second P-ROM is the concatenation of the code banks of all ELF files.

set(flags -std=c99 -fomit-frame-pointer -g)

ngdevkit_elf(14-prom-bankswitch-rom0
  SOURCES      main.c impl__bank0.c
  CFLAGS       ${flags}
  LINK_OPTIONS -Wl,--enable-non-contiguous-regions
)

ngdevkit_elf(14-prom-bankswitch-rom1
  SOURCES      main.c impl__bank1.c
  CFLAGS       ${flags}
  LINK_OPTIONS -Wl,--enable-non-contiguous-regions
)

ngdevkit_cartridge(
  GAMEROM     14_prom_bankswitch
  TITLE       "P-ROM bank switching"
  ELF         14-prom-bankswitch-rom0
  PROM2_BANKS 14-prom-bankswitch-rom0 14-prom-bankswitch-rom1
  PROM2SIZE   2097152
)
