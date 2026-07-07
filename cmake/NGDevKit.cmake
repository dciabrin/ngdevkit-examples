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

# Helper functions to build Neo-Geo cartridges with ngdevkit.
# This is the CMake counterpart of the Makefile-based build rules found
# in 00-template/{build.mk,rom.mk,emu.mk}. Both build systems coexist
# and are fully independent from each other.
#
# Each example directory contains a CMakeLists.txt that composes these
# helpers; generated files land in the example's binary directory:
#   gen/  compiled objects, converted assets, generated includes
#   rom/  the ROM chip files, the cartridge zip and emulator hash files

# Default chip sizes, as in 00-template/rom.mk
set(NGDK_PROMSIZE 1048576)
set(NGDK_CROMSIZE 2097152)
set(NGDK_SROMSIZE 131072)
set(NGDK_MROMSIZE 131072)
set(NGDK_VROMSIZE 524288)

set(NGDK_CONCAT ${CMAKE_CURRENT_LIST_DIR}/concat.py)

# Default compile flags for the m68k program, as in the template Makefile
set(NGDK_DEFAULT_CFLAGS -std=c99 -fomit-frame-pointer -O2 -g)


# ngdevkit_gen_dir()/ngdevkit_rom_dir(): per-example output directories
function(_ngdk_dirs)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/assets)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rom)
endfunction()


# ngdevkit_elf(<target> SOURCES <src...>
#              [CFLAGS <flags...>]          - replaces the default C flags
#              [LINK_OPTIONS <flags...>]
#              [GENERATED <files...>])      - generated headers/includes the
#                                             sources depend on
#
# Compiles an m68k ELF binary (the content of the program ROM). The
# example's gen/ directory is on the include path, like -I$(BUILDDIR)
# in the Makefile build.
function(ngdevkit_elf target)
  cmake_parse_arguments(ARG "" "" "SOURCES;CFLAGS;LINK_OPTIONS;GENERATED" ${ARGN})
  _ngdk_dirs()
  if(NOT ARG_CFLAGS)
    set(ARG_CFLAGS ${NGDK_DEFAULT_CFLAGS})
  endif()
  add_executable(${target} ${ARG_SOURCES} ${ARG_GENERATED})
  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME ${target}
    SUFFIX .elf
    # the Makefile build links C++ examples with gcc as well
    LINKER_LANGUAGE C)
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen)
  target_compile_options(${target} PRIVATE ${NGDEVKIT_CFLAGS} ${ARG_CFLAGS})
  # NGDEVKIT_LDFLAGS comes from `pkg-config --libs ngdevkit` and carries
  # -L, -specs and -l flags whose relative order must be preserved
  target_link_libraries(${target} PRIVATE ${NGDEVKIT_LDFLAGS})
  if(ARG_LINK_OPTIONS)
    target_link_options(${target} PRIVATE ${ARG_LINK_OPTIONS})
  endif()
endfunction()


# ngdevkit_fix_tiles(<outvar> <image>): fixed tiles from an image
function(ngdevkit_fix_tiles outvar image)
  _ngdk_dirs()
  get_filename_component(name ${image} NAME_WE)
  set(out ${CMAKE_CURRENT_BINARY_DIR}/gen/assets/${name}.fix)
  add_custom_command(OUTPUT ${out}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_TILETOOL} --fix -c ${image} -o ${out}
    DEPENDS ${image}
    COMMENT "Converting ${name} to fixed tiles"
    VERBATIM)
  set(${outvar} ${out} PARENT_SCOPE)
endfunction()


# ngdevkit_sprite_tiles(<c1var> <c2var> <image>): sprite tiles from an image
function(ngdevkit_sprite_tiles c1var c2var image)
  _ngdk_dirs()
  get_filename_component(name ${image} NAME_WE)
  set(c1 ${CMAKE_CURRENT_BINARY_DIR}/gen/assets/${name}.c1)
  set(c2 ${CMAKE_CURRENT_BINARY_DIR}/gen/assets/${name}.c2)
  add_custom_command(OUTPUT ${c1} ${c2}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_TILETOOL} --sprite -c ${image} -o ${c1} ${c2}
    DEPENDS ${image}
    COMMENT "Converting ${name} to sprite tiles"
    VERBATIM)
  set(${c1var} ${c1} PARENT_SCOPE)
  set(${c2var} ${c2} PARENT_SCOPE)
endfunction()


# ngdevkit_palette(<outvar> <image>): C palette data from an image,
# included by the program via "assets/<name>.pal"
function(ngdevkit_palette outvar image)
  _ngdk_dirs()
  get_filename_component(name ${image} NAME_WE)
  set(out ${CMAKE_CURRENT_BINARY_DIR}/gen/assets/${name}.pal)
  add_custom_command(OUTPUT ${out}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_PALTOOL} ${image} -o ${out}
    DEPENDS ${image}
    COMMENT "Extracting palette of ${name}"
    VERBATIM)
  set(${outvar} ${out} PARENT_SCOPE)
endfunction()


# ngdevkit_z80_rel(<outvar> <source.s> [OUTPUT <gen-relative path.rel>]
#                  [INCLUDE_DIRS <dirs...>] [DEPENDS <files/targets...>])
#
# Assembles a Z80 source file for the sound driver. The nullsound include
# directory and the example's gen/ directory are always on the include path.
function(ngdevkit_z80_rel outvar source)
  cmake_parse_arguments(ARG "" "OUTPUT" "INCLUDE_DIRS;DEPENDS" ${ARGN})
  _ngdk_dirs()
  if(NOT ARG_OUTPUT)
    get_filename_component(name ${source} NAME_WE)
    set(ARG_OUTPUT ${name}.rel)
  endif()
  set(out ${CMAKE_CURRENT_BINARY_DIR}/gen/${ARG_OUTPUT})
  set(incflags -I${NGDK_Z80INCLUDEDIR}/nullsound -I${CMAKE_CURRENT_BINARY_DIR}/gen)
  foreach(dir IN LISTS ARG_INCLUDE_DIRS)
    list(APPEND incflags -I${dir})
  endforeach()
  get_filename_component(outdir ${out} DIRECTORY)
  file(MAKE_DIRECTORY ${outdir})
  add_custom_command(OUTPUT ${out}
    COMMAND ${NGDK_Z80SDAS} -g -l -p -u ${incflags} -o ${out} ${source}
    DEPENDS ${source} ${ARG_DEPENDS}
    COMMENT "Assembling (z80) ${ARG_OUTPUT}"
    VERBATIM)
  set(${outvar} ${out} PARENT_SCOPE)
endfunction()


# ngdevkit_sound_driver(<outvar> NAME <name> INPUTS <rel/lib files...>
#                       [DEPENDS <targets...>])
#
# Links a sound driver against nullsound. The inputs are passed to the
# linker in the given order, after nullsound.lib.
function(ngdevkit_sound_driver outvar)
  cmake_parse_arguments(ARG "" "NAME" "INPUTS;DEPENDS" ${ARGN})
  _ngdk_dirs()
  set(out ${CMAKE_CURRENT_BINARY_DIR}/gen/${ARG_NAME}.ihx)
  add_custom_command(OUTPUT ${out}
    COMMAND ${NGDK_Z80SDLD} -b DATA=0xf800 -i ${out}
            ${NGDK_Z80LIBDIR}/nullsound.lib ${ARG_INPUTS}
    DEPENDS ${ARG_INPUTS} ${ARG_DEPENDS}
    COMMENT "Linking (z80) sound driver ${ARG_NAME}"
    VERBATIM)
  set(${outvar} ${out} PARENT_SCOPE)
endfunction()


# ngdevkit_vrom_from_yaml(<vromvar> GAMEROM <name> YAML <files...>
#                         SAMPLES_INC <gen-relative path>
#                         [STAGED <staged asset files...>])
#
# Builds the ADPCM sample ROM from asset map YAML files (vromtool), and
# generates the matching samples.inc for the sound driver. The YAML files
# reference their samples with paths relative to the example directory
# (e.g. file://assets/foo.wav); callers stage those (generated) assets
# under gen/ and list them in STAGED.
function(ngdevkit_vrom_from_yaml vromvar)
  cmake_parse_arguments(ARG "" "GAMEROM;SAMPLES_INC" "YAML;STAGED" ${ARGN})
  _ngdk_dirs()
  set(gen ${CMAKE_CURRENT_BINARY_DIR}/gen)
  set(samples_inc ${gen}/${ARG_SAMPLES_INC})
  set(vrom ${CMAKE_CURRENT_BINARY_DIR}/rom/${ARG_GAMEROM}-v1.v1)
  # the sample map: offsets of each sample in the VROM, for the z80 driver
  add_custom_command(OUTPUT ${samples_inc}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_VROMTOOL} --asm -s ${NGDK_VROMSIZE}
            ${ARG_YAML} -o ${gen}/vrom-scratch.bin -m ${samples_inc}
    DEPENDS ${ARG_YAML} ${ARG_STAGED}
    WORKING_DIRECTORY ${gen}
    COMMENT "Generating ${ARG_SAMPLES_INC}"
    VERBATIM)
  # the VROM itself
  add_custom_command(OUTPUT ${vrom}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_VROMTOOL} --roms -s ${NGDK_VROMSIZE}
            ${ARG_YAML} -o ${CMAKE_CURRENT_BINARY_DIR}/rom/${ARG_GAMEROM}-vX.vX -n 1
    DEPENDS ${ARG_YAML} ${ARG_STAGED} ${samples_inc}
    WORKING_DIRECTORY ${gen}
    COMMENT "Building sample ROM ${ARG_GAMEROM}-v1.v1"
    VERBATIM)
  set(${vromvar} ${vrom} PARENT_SCOPE)
endfunction()


# _ngdk_prom(<out> <elf-target> <objcopy args...>): common P-ROM recipe;
# extracts a binary from the ELF and byte-swaps it in place
function(_ngdk_prom out elf)
  get_filename_component(outname ${out} NAME)
  add_custom_command(OUTPUT ${out}
    COMMAND ${NGDK_M68KOBJCOPY} -O binary ${ARGN} --gap-fill 0xff
            $<TARGET_FILE:${elf}> ${out}
    COMMAND ${NGDK_DD} if=${out} of=${out} conv=notrunc,swab status=none
    DEPENDS ${elf}
    COMMENT "Building program ROM ${outname}"
    VERBATIM)
endfunction()


# ngdevkit_cartridge(
#   [NAME <target>]                  - defaults to the directory name
#   GAMEROM <romname> TITLE <title>
#   ELF <target>                     - program linked into PROM1
#   [PROM2_ELF <target>]             - PROM2 (.text2) from a single ELF
#   [PROM2_BANKS <targets...>]       - PROM2 as concatenated ELF banks
#   [PROM2SIZE <bytes>]
#   [SROM <fix files...>]            - defaults to the shared text tiles
#   [CROM1 <c1 files...>] [CROM2 <c2 files...>]
#                                    - default to the shared ngdevkit logo
#   [SOUND_DRIVER <ihx file>]        - defaults to the shared base driver
#   [VROM <v1 file>]                 - defaults to an empty sample ROM
#   [MEMCARD]                        - provide a memory card to emulators
#   [DEPENDS <extra targets...>])
#
# Assembles all ROM chips into a cartridge zip plus the hash files used
# by MAME and GnGeo, copies the BIOSes next to it, and creates emulator
# run targets (<name>-gngeo, <name>-gngeo-mvs, <name>-mame, <name>-mame-mvs).
function(ngdevkit_cartridge)
  cmake_parse_arguments(ARG "MEMCARD" "NAME;GAMEROM;TITLE;ELF;PROM2_ELF;PROM2SIZE"
    "PROM2_BANKS;SROM;CROM1;CROM2;SOUND_DRIVER;VROM;DEPENDS" ${ARGN})
  _ngdk_dirs()
  set(rom ${CMAKE_CURRENT_BINARY_DIR}/rom)
  set(game ${ARG_GAMEROM})
  if(NOT ARG_NAME)
    get_filename_component(ARG_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  endif()
  if(NOT ARG_SROM)
    set(ARG_SROM ${NGDK_BASE_SROM_FIX})
  endif()
  if(NOT ARG_CROM1)
    set(ARG_CROM1 ${NGDK_BASE_CROM_C1})
    set(ARG_CROM2 ${NGDK_BASE_CROM_C2})
  endif()
  if(NOT ARG_SOUND_DRIVER)
    set(ARG_SOUND_DRIVER ${NGDK_BASE_SOUND_DRIVER})
  endif()
  # every consumed file may come from the shared assets directory scope,
  # so all custom commands also carry a target-level dependency
  set(deps ${NGDK_SHARED_ASSETS_TARGET} ${ARG_DEPENDS})

  # program ROM(s)
  set(prom1 ${rom}/${game}-p1.p1)
  _ngdk_prom(${prom1} ${ARG_ELF} -S -R .text2 --pad-to ${NGDK_PROMSIZE})
  set(proms ${prom1})
  math(EXPR prom2_pad "2097152 + ${NGDK_PROMSIZE}")
  if(ARG_PROM2_ELF)
    set(prom2 ${rom}/${game}-p2.p2)
    _ngdk_prom(${prom2} ${ARG_PROM2_ELF} -j .text2 --pad-to ${prom2_pad})
    list(APPEND proms ${prom2})
  elseif(ARG_PROM2_BANKS)
    set(prom2 ${rom}/${game}-p2.p2)
    set(banks)
    set(banknum 0)
    foreach(elf IN LISTS ARG_PROM2_BANKS)
      set(bank ${prom2}_bank${banknum})
      _ngdk_prom(${bank} ${elf} -j .text2 --pad-to ${prom2_pad})
      list(APPEND banks ${bank})
      math(EXPR banknum "${banknum} + 1")
    endforeach()
    add_custom_command(OUTPUT ${prom2}
      COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size ${ARG_PROM2SIZE} ${prom2} ${banks}
      DEPENDS ${banks}
      COMMENT "Building banked program ROM ${game}-p2.p2"
      VERBATIM)
    list(APPEND proms ${prom2})
  endif()

  # fixed tiles ROM
  set(srom ${rom}/${game}-s1.s1)
  add_custom_command(OUTPUT ${srom}
    COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size ${NGDK_SROMSIZE} ${srom} ${ARG_SROM}
    DEPENDS ${ARG_SROM} ${deps}
    COMMENT "Building fixed tiles ROM ${game}-s1.s1"
    VERBATIM)

  # sprite tiles ROMs
  set(crom1 ${rom}/${game}-c1.c1)
  set(crom2 ${rom}/${game}-c2.c2)
  add_custom_command(OUTPUT ${crom1}
    COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size ${NGDK_CROMSIZE} ${crom1} ${ARG_CROM1}
    DEPENDS ${ARG_CROM1} ${deps}
    COMMENT "Building sprite ROM ${game}-c1.c1"
    VERBATIM)
  add_custom_command(OUTPUT ${crom2}
    COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size ${NGDK_CROMSIZE} ${crom2} ${ARG_CROM2}
    DEPENDS ${ARG_CROM2} ${deps}
    COMMENT "Building sprite ROM ${game}-c2.c2"
    VERBATIM)

  # sound driver ROM
  set(mrom ${rom}/${game}-m1.m1)
  add_custom_command(OUTPUT ${mrom}
    COMMAND ${NGDK_Z80SDOBJCOPY} -I ihex -O binary ${ARG_SOUND_DRIVER} ${mrom}
            --pad-to ${NGDK_MROMSIZE}
    DEPENDS ${ARG_SOUND_DRIVER} ${deps}
    COMMENT "Building sound driver ROM ${game}-m1.m1"
    VERBATIM)

  # ADPCM samples ROM
  if(ARG_VROM)
    set(vrom ${ARG_VROM})
  else()
    set(vrom ${rom}/${game}-v1.v1)
    add_custom_command(OUTPUT ${vrom}
      COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size ${NGDK_VROMSIZE} ${vrom}
      COMMENT "Building empty sample ROM ${game}-v1.v1"
      VERBATIM)
  endif()

  set(chips ${proms} ${srom} ${crom1} ${crom2} ${mrom} ${vrom})
  set(chipargs -p ${proms} -c ${crom1} ${crom2} -v ${vrom} -s ${srom} -m ${mrom})

  # cartridge zip and emulator hash files
  set(cart ${rom}/${game}.zip)
  add_custom_command(OUTPUT ${cart}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_ROMTOOL} -b cartridge -f zip ${chipargs}
            -n ${game} -x "zip.comment=NGDEVKIT example ROM - https://github.com/dciabrin/ngdevkit"
            -o ${cart}
    DEPENDS ${chips} ${deps}
    COMMENT "Building cartridge ${game}.zip"
    VERBATIM)
  set(mame_hash ${rom}/neogeo.xml)
  add_custom_command(OUTPUT ${mame_hash}
    COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_ROMTOOL} -b hash -f mame ${chipargs}
            -n ${game} -l ${ARG_TITLE} -o ${mame_hash}
    DEPENDS ${chips} ${cart} ${deps}
    COMMENT "Building MAME hash file for ${game}"
    VERBATIM)
  set(arts ${cart} ${mame_hash})
  if(NGDK_GNGEO_DATA)
    set(gngeo_hash ${rom}/gngeo_data.zip)
    add_custom_command(OUTPUT ${gngeo_hash}
      COMMAND ${NGDK_TOOL_LAUNCHER} ${NGDK_ROMTOOL} -b hash -f gngeo ${chipargs}
              -n ${game} -l ${ARG_TITLE} -x gngeo.data=${NGDK_GNGEO_DATA} -o ${gngeo_hash}
      DEPENDS ${chips} ${cart} ${deps}
      COMMENT "Building GnGeo hash file for ${game}"
      VERBATIM)
    list(APPEND arts ${gngeo_hash})
  endif()

  # BIOS ROMs next to the cartridge (nullbios by default)
  foreach(bios IN LISTS NGDK_AES_BIOS NGDK_MVS_BIOS)
    get_filename_component(biosname ${bios} NAME)
    add_custom_command(OUTPUT ${rom}/${biosname}
      COMMAND ${CMAKE_COMMAND} -E copy ${bios} ${rom}/${biosname}
      DEPENDS ${bios}
      COMMENT "Installing BIOS ${biosname}"
      VERBATIM)
    list(APPEND arts ${rom}/${biosname})
  endforeach()

  # memory card
  set(memcard)
  set(mame_memcard_flags)
  if(ARG_MEMCARD)
    set(memcard ${CMAKE_CURRENT_BINARY_DIR}/card.mc)
    add_custom_command(OUTPUT ${memcard}
      COMMAND ${NGDK_PYTHON} ${NGDK_CONCAT} --size 2048 ${memcard}
      COMMENT "Creating empty memory card"
      VERBATIM)
    set(mame_memcard_flags -memc ${memcard})
    list(APPEND arts ${memcard})
  endif()

  add_custom_target(${ARG_NAME} ALL DEPENDS ${arts})

  # emulator run targets
  if(NGDK_GNGEO)
    set(gngeo_cmd ${CMAKE_COMMAND} -E env ${NGDK_EMU_ENV} ${NGDK_GNGEO}
        ${NGDK_GNGEO_SHADEROPTS} --scale 3 --no-resize
        -i ${rom} -d ${rom}/gngeo_data.zip ${game})
    add_custom_target(${ARG_NAME}-gngeo
      COMMAND ${gngeo_cmd} --system home
      DEPENDS ${ARG_NAME} USES_TERMINAL VERBATIM)
    add_custom_target(${ARG_NAME}-gngeo-mvs
      COMMAND ${gngeo_cmd} --system arcade
      DEPENDS ${ARG_NAME} USES_TERMINAL VERBATIM)
  endif()
  math(EXPR mame_w "3 * 320")
  math(EXPR mame_h "3 * 224")
  set(mame_cmd ${NGDK_MAME} -w -resolution ${mame_w}x${mame_h} -noautosave
      -skip_gameinfo -hash ${rom} -rp ${rom})
  add_custom_target(${ARG_NAME}-mame
    COMMAND ${mame_cmd} aes ${mame_memcard_flags} -cart ${game}
    DEPENDS ${ARG_NAME} USES_TERMINAL VERBATIM)
  add_custom_target(${ARG_NAME}-mame-mvs
    COMMAND ${mame_cmd} neogeo ${mame_memcard_flags} -cart1 ${game}
    DEPENDS ${ARG_NAME} USES_TERMINAL VERBATIM)
endfunction()
