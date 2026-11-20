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
set(gfx ${CMAKE_CURRENT_SOURCE_DIR}/setup/prepare-hero/gfx)

# the original files for this demo need to get their transparent color fixed
foreach(anim IN ITEMS idle walk)
  add_custom_command(OUTPUT ${gen}/${anim}.gif
    COMMAND ${NGDK_SH} -c "cp '${gfx}/spr_m_traveler_${anim}_anim.gif' '${gen}/${anim}.gif' && printf '\\xff\\x00\\xff' | dd of='${gen}/${anim}.gif' bs=1 seek=13 count=3 conv=notrunc status=none"
    DEPENDS ${gfx}/spr_m_traveler_${anim}_anim.gif
    COMMENT "Preparing ${anim}.gif"
    VERBATIM)
endforeach()

# we want all the sprites to share the same palette, so for simplicity
# in this demo we merge all the frames into a single file with imagemagick
add_custom_command(OUTPUT ${gen}/assets/all.gif
  COMMAND ${NGDK_CONVERT} ${gen}/idle.gif ${gen}/walk.gif -coalesce -append
          +repage "xc:#ff00ff[64x1024!]" -compose dstover -compose dstover
          -fill "#00ffff" -opaque "#000100" -fill "#00ffff" -opaque "#000000"
          -fill "#181818" -opaque "#00ffff" -fill "#000000" -opaque "#ff00ff"
          -composite ${gen}/assets/all.gif
  DEPENDS ${gen}/idle.gif ${gen}/walk.gif
  COMMENT "Merging animation frames into all.gif"
  VERBATIM)

ngdevkit_sprite_tiles(ALL_C1 ALL_C2 ${gen}/assets/all.gif)
ngdevkit_palette(ALL_PAL ${gen}/assets/all.gif)

ngdevkit_elf(03-sprite-animation-rom
  SOURCES   main.c
  GENERATED ${ALL_PAL}
)

ngdevkit_cartridge(
  GAMEROM 03_sprite_anim
  TITLE   "Sprite animation"
  ELF     03-sprite-animation-rom
  CROM1   ${NGDK_BASE_CROM_C1} ${ALL_C1}
  CROM2   ${NGDK_BASE_CROM_C2} ${ALL_C2}
)
