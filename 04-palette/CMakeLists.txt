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

# palette data for all the gradients displayed by the example
add_custom_command(OUTPUT ${gen}/gradients.pal
  COMMAND ${NGDK_SH} -c "'${NGDK_PYTHON}' '${CMAKE_CURRENT_SOURCE_DIR}/mk-gradients.py' > '${gen}/gradients.pal'"
  DEPENDS mk-gradients.py
  COMMENT "Generating gradients.pal"
  VERBATIM)

# a gradient image, converted to fixed tiles to visualize the palettes
add_custom_command(OUTPUT ${gen}/assets/gradient.gif
  COMMAND ${NGDK_CONVERT} -size 16x1 gradient:black-white -scale 800%
          ${gen}/assets/gradient.gif
  COMMENT "Generating gradient.gif"
  VERBATIM)
ngdevkit_fix_tiles(GRADIENT_FIX ${gen}/assets/gradient.gif)

ngdevkit_elf(04-palette-rom
  SOURCES   main.c
  GENERATED ${gen}/gradients.pal
)

ngdevkit_cartridge(
  GAMEROM 03_palette
  TITLE   "Palette test"
  ELF     04-palette-rom
  SROM    ${NGDK_BASE_SROM_FIX} ${GRADIENT_FIX}
)
