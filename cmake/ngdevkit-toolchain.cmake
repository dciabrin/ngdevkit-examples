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

# Cross-compilation toolchain for the Neo-Geo main CPU (Motorola 68000).
# The top-level CMakeLists.txt uses this file automatically; it can also
# be passed explicitly with -DCMAKE_TOOLCHAIN_FILE=.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

find_program(NGDK_M68K_GCC m68k-neogeo-elf-gcc REQUIRED)
find_program(NGDK_M68K_GXX m68k-neogeo-elf-g++ REQUIRED)

set(CMAKE_C_COMPILER ${NGDK_M68K_GCC})
set(CMAKE_CXX_COMPILER ${NGDK_M68K_GXX})

# A plain test executable cannot link without the ngdevkit specs/libraries,
# so probe the compilers by building a static library instead.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# All support tools (python tools, sdcc toolchain, emulators...) are host
# programs found in PATH.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)
