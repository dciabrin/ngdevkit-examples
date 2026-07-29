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

set(gen ${CMAKE_CURRENT_BINARY_DIR}/gen)

# precomputed wobble offsets for the horizontal sync effect
add_custom_command(OUTPUT ${gen}/precalc.inc
  COMMAND ${NGDK_SH} -c "'${NGDK_PYTHON}' '${CMAKE_CURRENT_SOURCE_DIR}/gen-wobble.py' 26 70 64 > '${gen}/precalc.inc'"
  DEPENDS gen-wobble.py
  COMMENT "Generating precalc.inc"
  VERBATIM)

# the ngdevkit logo as a movable sprite
set(logo ${CMAKE_CURRENT_BINARY_DIR}/gen/assets/logo.gif)
add_custom_command(OUTPUT ${logo}
  COMMAND ${CMAKE_COMMAND} -E copy ${NGDK_LOGO_GIF} ${logo}
  DEPENDS ${NGDK_LOGO_GIF}
  VERBATIM)
ngdevkit_sprite_tiles(LOGO_C1 LOGO_C2 ${logo})

ngdevkit_elf(09-horizontal-sync-rom
  SOURCES   main.c
  GENERATED ${gen}/precalc.inc
)

ngdevkit_cartridge(
  GAMEROM 09_hsync
  TITLE   "Horizontal sync"
  ELF     09-horizontal-sync-rom
  CROM1   ${NGDK_BASE_CROM_C1} ${LOGO_C1}
  CROM2   ${NGDK_BASE_CROM_C2} ${LOGO_C2}
)
