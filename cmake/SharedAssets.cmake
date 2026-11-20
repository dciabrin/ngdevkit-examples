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

# Builds the ngdevkit assets shared by all examples (text tiles, the
# ngdevkit logo sprites, the base sound driver and the eye-catcher
# music library). This is the CMake counterpart of
# 00-template/setup/ngdevkit-assets/Makefile; the Makefile build runs
# that once per configure and rsyncs the results into every example,
# whereas here every example simply depends on the files built in
# ${NGDK_SHARED_DIR}.

set(NGDK_SHARED_DIR ${CMAKE_BINARY_DIR}/ngdevkit-assets)
set(_src ${CMAKE_SOURCE_DIR}/00-template/setup/ngdevkit-assets)
file(MAKE_DIRECTORY ${NGDK_SHARED_DIR})

# --- fixed tiles ROM data: small and tall latin fonts from unscii ---

foreach(size IN ITEMS 8 16)
  if(size EQUAL 8)
    set(name smalltext)
    set(crop 256x24+2+1)
    set(tile 8x8)
    set(extent 1024x8)
  else()
    set(name talltext)
    set(crop 256x48+2+1)
    set(tile 8x16)
    set(extent 1024x16)
  endif()
  add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/${name}.gif
    COMMAND ${NGDK_CONVERT} ${_src}/gfx/unscii${size}.png -crop ${crop} +repage
            -crop ${tile} +append +repage -background black -gravity east
            -extent ${extent} -fuzz 30% -fill white -opaque white -colors 2
            ${NGDK_SHARED_DIR}/${name}.gif
    DEPENDS ${_src}/gfx/unscii${size}.png
    COMMENT "Generating ${name}.gif"
    VERBATIM)
  add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/${name}-shadow.gif
    COMMAND ${NGDK_CONVERT} -size ${extent} xc:black
            "(" ${NGDK_SHARED_DIR}/${name}.gif -transparent black -fuzz 30% -fill "#ffffff" -opaque white ")"
            -geometry +1+1 -composite
            "(" ${NGDK_SHARED_DIR}/${name}.gif -transparent black -fuzz 30% -fill "#ff0000" -opaque white -geometry +0+0 ")"
            -composite -colors 16 ${NGDK_SHARED_DIR}/${name}-shadow.gif
    DEPENDS ${NGDK_SHARED_DIR}/${name}.gif
    COMMENT "Generating ${name}-shadow.gif"
    VERBATIM)
endforeach()

foreach(variant IN ITEMS "" "-shadow")
  if(variant STREQUAL "")
    set(small smalltext.gif)
    set(tall talltext.gif)
    set(fix base-srom-text.fix)
  else()
    set(small smalltext-shadow.gif)
    set(tall talltext-shadow.gif)
    set(fix base-srom-text-shadow.fix)
  endif()
  add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/srom${variant}.bmp
    COMMAND ${NGDK_PYTHON} ${_src}/build-srom.py
            -s ${NGDK_SHARED_DIR}/${small} -t ${NGDK_SHARED_DIR}/${tall}
            -b ${NGDK_SHARED_DIR}/talltext.gif -o ${NGDK_SHARED_DIR}/srom${variant}.bmp
    DEPENDS ${_src}/build-srom.py ${NGDK_SHARED_DIR}/${small}
            ${NGDK_SHARED_DIR}/${tall} ${NGDK_SHARED_DIR}/talltext.gif
    COMMENT "Generating srom${variant}.bmp"
    VERBATIM)
  add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/${fix}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_TILETOOL} --fix
            -c ${NGDK_SHARED_DIR}/srom${variant}.bmp -o ${NGDK_SHARED_DIR}/${fix}
    DEPENDS ${NGDK_SHARED_DIR}/srom${variant}.bmp
    COMMENT "Generating ${fix}"
    VERBATIM)
endforeach()
set(NGDK_BASE_SROM_FIX ${NGDK_SHARED_DIR}/base-srom-text-shadow.fix)

# --- sprite ROM data: the ngdevkit logo used during attract mode ---
# tile 14 of the converted logo is dropped, and the result is padded,
# exactly like the dd-based recipe in the Makefile build
set(NGDK_BASE_CROM_C1 ${NGDK_SHARED_DIR}/base-crom-logo.c1)
set(NGDK_BASE_CROM_C2 ${NGDK_SHARED_DIR}/base-crom-logo.c2)
add_custom_command(OUTPUT ${NGDK_BASE_CROM_C1} ${NGDK_BASE_CROM_C2}
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_TILETOOL} --sprite -c ${_src}/gfx/logo.gif
          -o tmp.c1 tmp.c2
  COMMAND ${NGDK_SH} -c "(dd bs=64 count=14 if=tmp.c1; dd bs=64 skip=15 if=tmp.c1; dd bs=64 count=197 if=/dev/zero) 2>/dev/null | cat > base-crom-logo.c1"
  COMMAND ${NGDK_SH} -c "(dd bs=64 count=14 if=tmp.c2; dd bs=64 skip=15 if=tmp.c2; dd bs=64 count=197 if=/dev/zero) 2>/dev/null | cat > base-crom-logo.c2"
  DEPENDS ${_src}/gfx/logo.gif
  WORKING_DIRECTORY ${NGDK_SHARED_DIR}
  COMMENT "Generating base-crom-logo.c1/.c2"
  VERBATIM)

# --- eye-catcher music library and base sound driver ---

add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/samples.inc
  COMMAND ${CMAKE_COMMAND} -E touch ${NGDK_SHARED_DIR}/samples.inc
  COMMENT "Generating empty samples.inc"
  VERBATIM)

add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/nss-ngdevkit.s
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_NSSTOOL} -z -n nss_ngdevkit
          ${_src}/music/ngdevkit.fur -o ${NGDK_SHARED_DIR}/nss-ngdevkit.s
  DEPENDS ${_src}/music/ngdevkit.fur
  COMMENT "Generating nss-ngdevkit.s"
  VERBATIM)

add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/instruments-ngdevkit.s
  COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_FURTOOL} ${_src}/music/ngdevkit.fur
          --instruments -n instruments_ngdevkit -o ${NGDK_SHARED_DIR}/instruments-ngdevkit.s
  DEPENDS ${_src}/music/ngdevkit.fur ${NGDK_SHARED_DIR}/samples.inc
  COMMENT "Generating instruments-ngdevkit.s"
  VERBATIM)

set(_rels)
foreach(src IN ITEMS ${_src}/eye-catcher.s
                     ${NGDK_SHARED_DIR}/instruments-ngdevkit.s
                     ${NGDK_SHARED_DIR}/nss-ngdevkit.s)
  get_filename_component(relname ${src} NAME_WE)
  set(rel ${NGDK_SHARED_DIR}/${relname}.rel)
  add_custom_command(OUTPUT ${rel}
    COMMAND ${NGDK_Z80SDAS} -g -l -p -u -I${NGDK_Z80INCLUDEDIR}/nullsound
            -I${NGDK_SHARED_DIR} -I${_src} -o ${rel} ${src}
    DEPENDS ${src} ${NGDK_SHARED_DIR}/samples.inc
    COMMENT "Assembling (z80) ${relname}.rel"
    VERBATIM)
  list(APPEND _rels ${rel})
endforeach()

# the lib below can be reused by an example's sound driver to embed
# ngdevkit's attract music
set(NGDK_EYE_CATCHER_LIB ${NGDK_SHARED_DIR}/ngdevkit-eye-catcher.lib)
add_custom_command(OUTPUT ${NGDK_EYE_CATCHER_LIB}
  COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} ${NGDK_EYE_CATCHER_LIB} ${_rels}
  DEPENDS ${_rels}
  COMMENT "Generating ngdevkit-eye-catcher.lib"
  VERBATIM)

set(NGDK_BASE_SOUND_DRIVER ${NGDK_SHARED_DIR}/base-sound-driver.ihx)
add_custom_command(OUTPUT ${NGDK_SHARED_DIR}/base-sound-driver.rel
  COMMAND ${NGDK_Z80SDAS} -g -l -p -u -I${NGDK_Z80INCLUDEDIR}/nullsound
          -I${NGDK_SHARED_DIR} -I${_src} -o ${NGDK_SHARED_DIR}/base-sound-driver.rel
          ${_src}/base-sound-driver.s
  DEPENDS ${_src}/base-sound-driver.s
  COMMENT "Assembling (z80) base-sound-driver.rel"
  VERBATIM)
add_custom_command(OUTPUT ${NGDK_BASE_SOUND_DRIVER}
  COMMAND ${NGDK_Z80SDLD} -b DATA=0xf800 -m -w -i ${NGDK_BASE_SOUND_DRIVER}
          ${NGDK_Z80LIBDIR}/nullsound.lib ${NGDK_EYE_CATCHER_LIB}
          ${NGDK_SHARED_DIR}/base-sound-driver.rel
  DEPENDS ${NGDK_EYE_CATCHER_LIB} ${NGDK_SHARED_DIR}/base-sound-driver.rel
  COMMENT "Linking (z80) base-sound-driver.ihx"
  VERBATIM)

# target-level anchor: custom command outputs are only visible in this
# directory scope, examples depend on this target to bridge that
set(NGDK_SHARED_ASSETS_TARGET ngdevkit-shared-assets)
add_custom_target(${NGDK_SHARED_ASSETS_TARGET}
  DEPENDS ${NGDK_BASE_SROM_FIX} ${NGDK_SHARED_DIR}/base-srom-text.fix
          ${NGDK_BASE_CROM_C1} ${NGDK_BASE_CROM_C2}
          ${NGDK_BASE_SOUND_DRIVER} ${NGDK_EYE_CATCHER_LIB})

# the shared ngdevkit logo, reused as sprite input by some examples
set(NGDK_LOGO_GIF ${_src}/gfx/logo.gif)

unset(_src)
unset(_rels)
