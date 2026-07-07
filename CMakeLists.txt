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

# CMake build for the ngdevkit examples. This build system coexists with
# the autotools/make one; both are fully independent, use whichever you
# prefer:
#   cmake -B build
#   cmake --build build
# Emulator run targets: cmake --build build --target 01-helloworld-gngeo
# (also: -gngeo-mvs, -mame, -mame-mvs)

cmake_minimum_required(VERSION 3.21)

# The examples target the Neo-Geo m68k CPU; default to the bundled
# toolchain file so a plain `cmake -B build` works out of the box.
if(NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ngdevkit-toolchain.cmake)
endif()

project(ngdevkit-examples LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# --- ngdevkit itself ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(NGDEVKIT REQUIRED "ngdevkit>=0.5")
# `pkg-config --libs` yields `-specs ngdevkit`; fold the two-token form
# into -specs=<file> so CMake does not mistake the argument for a library
set(_ldflags)
set(_pending_specs FALSE)
foreach(flag IN LISTS NGDEVKIT_LDFLAGS)
  if(_pending_specs)
    list(APPEND _ldflags "-specs=${flag}")
    set(_pending_specs FALSE)
  elseif(flag MATCHES "^--?specs$")
    set(_pending_specs TRUE)
  else()
    list(APPEND _ldflags ${flag})
  endif()
endforeach()
set(NGDEVKIT_LDFLAGS ${_ldflags})
pkg_get_variable(NGDK_BINDIR ngdevkit bindir)
pkg_get_variable(NGDK_SHAREDIR ngdevkit sharedir)
pkg_get_variable(NGDK_LIBDIR ngdevkit libdir)
pkg_get_variable(NGDK_Z80INCLUDEDIR ngdevkit z80includedir)
pkg_get_variable(NGDK_Z80LIBDIR ngdevkit z80libdir)

# --- toolchains and tools ---
find_program(NGDK_M68KOBJCOPY m68k-neogeo-elf-objcopy REQUIRED)
find_program(NGDK_Z80SDAS z80-neogeo-ihx-sdasz80 REQUIRED)
find_program(NGDK_Z80SDLD z80-neogeo-ihx-sdldz80 REQUIRED)
find_program(NGDK_Z80SDOBJCOPY z80-neogeo-ihx-sdobjcopy REQUIRED)

find_program(NGDK_PYTHON NAMES python3 python REQUIRED)
foreach(tool IN ITEMS paltool tiletool adpcmtool vromtool furtool nsstool soundtool romtool)
  string(TOUPPER ${tool} TOOL)
  find_program(NGDK_${TOOL} ${tool}.py HINTS ${NGDK_BINDIR} REQUIRED)
endforeach()
# On Windows the python tools cannot be executed via their shebang;
# launch them through the python interpreter there.
if(WIN32)
  set(NGDK_TOOL_LAUNCHER ${NGDK_PYTHON})
else()
  set(NGDK_TOOL_LAUNCHER "")
endif()

find_program(NGDK_CONVERT NAMES magick convert REQUIRED)
find_program(NGDK_SOX sox REQUIRED)
find_program(NGDK_DD dd REQUIRED)
find_program(NGDK_SH sh REQUIRED)

# --- BIOS ROMs used to run the examples (nullbios by default) ---
set(AES_BIOS "" CACHE FILEPATH "Absolute path to the AES BIOS file to use")
set(MVS_BIOS "" CACHE FILEPATH "Absolute path to the MVS BIOS file to use")
if(AES_BIOS)
  set(NGDK_AES_BIOS ${AES_BIOS})
else()
  set(NGDK_AES_BIOS ${NGDK_SHAREDIR}/aes.zip)
endif()
if(MVS_BIOS)
  set(NGDK_MVS_BIOS ${MVS_BIOS})
else()
  set(NGDK_MVS_BIOS ${NGDK_SHAREDIR}/neogeo.zip)
endif()

# --- emulators ---
find_program(NGDK_GNGEO ngdevkit-gngeo)
find_program(NGDK_MAME mame)
if(NOT NGDK_MAME)
  set(NGDK_MAME mame)
endif()

# GnGeo dynamically loads the ngdevkit runtime library
if(APPLE)
  set(NGDK_EMU_ENV "DYLD_LIBRARY_PATH=${NGDK_LIBDIR}:$ENV{DYLD_LIBRARY_PATH}")
else()
  set(NGDK_EMU_ENV "LD_LIBRARY_PATH=${NGDK_LIBDIR}:$ENV{LD_LIBRARY_PATH}")
endif()

set(NGDK_GNGEO_DATA "")
set(NGDK_GNGEO_SHADEROPTS "")
if(NGDK_GNGEO)
  # locate GnGeo's data file (holds its default skin and shaders)
  execute_process(COMMAND ${NGDK_GNGEO} "" forcefail
    OUTPUT_VARIABLE _gngeo_out ERROR_VARIABLE _gngeo_err TIMEOUT 10)
  if("${_gngeo_out}\n${_gngeo_err}" MATCHES "DATAFILE = ([^\r\n]+)")
    set(NGDK_GNGEO_DATA ${CMAKE_MATCH_1})
    if(DEFINED ENV{MSYSTEM})
      find_program(NGDK_CYGPATH cygpath)
      if(NGDK_CYGPATH)
        execute_process(COMMAND ${NGDK_CYGPATH} -u ${NGDK_GNGEO_DATA}
          OUTPUT_VARIABLE NGDK_GNGEO_DATA OUTPUT_STRIP_TRAILING_WHITESPACE)
      endif()
    endif()
    message(STATUS "Found GnGeo data file: ${NGDK_GNGEO_DATA}")
  else()
    message(WARNING "Could not detect GnGeo's data file; GnGeo targets disabled")
    set(NGDK_GNGEO "")
  endif()
endif()
if(NGDK_GNGEO)
  # is GnGeo built with the GLSL shader blitter?
  execute_process(COMMAND ${NGDK_GNGEO} -b help
    OUTPUT_VARIABLE _gngeo_blitters ERROR_VARIABLE _ignored TIMEOUT 10)
  if(_gngeo_blitters MATCHES "(^|\n)glsl")
    set(NGDK_GNGEO_GLSL yes)
  else()
    set(NGDK_GNGEO_GLSL no)
  endif()
  message(STATUS "GnGeo GLSL blitter: ${NGDK_GNGEO_GLSL}")

  # pick the CRT shader from the git submodule when available
  set(NGDK_SHADER_PATH "")
  set(NGDK_SHADER noop.glslp)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/qcrt-flat.glslp)
    set(NGDK_SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    set(NGDK_SHADER qcrt-flat.glslp)
  endif()
  if(NGDK_GNGEO_GLSL STREQUAL yes)
    set(NGDK_GNGEO_SHADEROPTS -b glsl)
    if(NGDK_SHADER_PATH)
      list(APPEND NGDK_GNGEO_SHADEROPTS --shaderpath=${NGDK_SHADER_PATH})
    endif()
    list(APPEND NGDK_GNGEO_SHADEROPTS --shader=${NGDK_SHADER})
  endif()

  # generate a default input config for GnGeo if none exists yet
  if(DEFINED ENV{HOME})
    set(_gngeo_cfg "$ENV{HOME}/.gngeo/ngdevkit-gngeorc")
    if(NOT EXISTS ${_gngeo_cfg})
      message(STATUS "Generating a default input config for GnGeo")
      if(NGDK_GNGEO_GLSL STREQUAL yes)
        set(_cfg "# default blitter\nblitter glsl\n")
        if(NGDK_SHADER_PATH)
          string(APPEND _cfg "shaderpath ${NGDK_SHADER_PATH}\n")
        endif()
        string(APPEND _cfg "shader ${NGDK_SHADER}\n")
      else()
        set(_cfg "# default blitter\nblitter soft\n")
      endif()
      string(APPEND _cfg "
# default scale factor
scale 3

# default key mapping
p1control A=K97,B=K115,C=K113,D=K119,START=K49,COIN=K51,UP=K82,DOWN=K81,LEFT=K80,RIGHT=K79,A=J0B0,B=J0B1,C=J0B2,D=J0B3,START=J0B9,COIN=J0B8,UP=J0a3,DOWN=J0a3,LEFT=J0A0,RIGHT=J0A0
p2control A=K103,B=K104,C=K116,D=K117,START=K50,COIN=K52,UP=K105,DOWN=K107,LEFT=K106,RIGHT=K108,A=J1B0,B=J1B1,C=J1B2,D=J1B3,START=J1B9,COIN=J1B8,UP=J1a3,DOWN=J1a3,LEFT=J1A0,RIGHT=J1A0
")
      file(WRITE ${_gngeo_cfg} "${_cfg}")
    endif()
  endif()
endif()

include(NGDevKit)
include(SharedAssets)

add_subdirectory(00-template)
add_subdirectory(01-helloworld)
add_subdirectory(02-sprite)
add_subdirectory(03-sprite-animation)
add_subdirectory(04-palette)
add_subdirectory(05-scrolling)
add_subdirectory(06-sound-adpcma)
add_subdirectory(07-attract-and-game)
add_subdirectory(08-software-dips)
add_subdirectory(09-horizontal-sync)
add_subdirectory(10-credits-management)
add_subdirectory(11-backup-ram)
add_subdirectory(12-prom-1mb)
add_subdirectory(13-prom-full-2mb)
add_subdirectory(14-prom-bankswitch)
add_subdirectory(15-sound-adpcmb)
add_subdirectory(16-sound-music)
add_subdirectory(17-hello-c++)
add_subdirectory(18-memory-card)
