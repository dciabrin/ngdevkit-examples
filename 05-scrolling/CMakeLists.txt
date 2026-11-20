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
set(gfx ${CMAKE_CURRENT_SOURCE_DIR}/setup/prepare-backgrounds/gfx)

# the original art for this demo needs to get cropped to a specific size

add_custom_command(OUTPUT ${gen}/assets/back.gif
  COMMAND ${NGDK_CONVERT} ${gfx}/country-platform-back.png
          -crop 384x144+0+16 +repage ${gen}/assets/back.gif
  DEPENDS ${gfx}/country-platform-back.png
  COMMENT "Preparing back.gif"
  VERBATIM)

add_custom_command(OUTPUT ${gen}/assets/forest.gif
  COMMAND ${NGDK_CONVERT} ${gfx}/country-platform-forest.png
          ${gfx}/country-platform-forest.png ${gfx}/country-platform-forest.png
          +append -crop 480x144+0+80 +repage -background black -flatten
          ${gen}/assets/forest.gif
  DEPENDS ${gfx}/country-platform-forest.png
  COMMENT "Preparing forest.gif"
  VERBATIM)

add_custom_command(OUTPUT ${gen}/assets/front.gif
  COMMAND ${NGDK_CONVERT} ${gfx}/country-platform-tiles-example.png
          ${gfx}/country-platform-tiles-example.png
          +append -crop 384x144+0+80 +repage -background black -flatten
          ${gen}/assets/front.gif
  DEPENDS ${gfx}/country-platform-tiles-example.png
  COMMENT "Preparing front.gif"
  VERBATIM)

set(pals)
foreach(layer IN ITEMS back forest front)
  string(TOUPPER ${layer} LAYER)
  ngdevkit_sprite_tiles(${LAYER}_C1 ${LAYER}_C2 ${gen}/assets/${layer}.gif)
  ngdevkit_palette(${LAYER}_PAL ${gen}/assets/${layer}.gif)
  list(APPEND pals ${${LAYER}_PAL})
endforeach()

ngdevkit_elf(05-scrolling-rom
  SOURCES   main.c
  GENERATED ${pals}
)

ngdevkit_cartridge(
  GAMEROM 05_scrolling
  TITLE   "Parallax scrolling"
  ELF     05-scrolling-rom
  CROM1   ${NGDK_BASE_CROM_C1} ${BACK_C1} ${FOREST_C1} ${FRONT_C1}
  CROM2   ${NGDK_BASE_CROM_C2} ${BACK_C2} ${FOREST_C2} ${FRONT_C2}
)
