# Copyright (c) 2019 Damien Ciabrini
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

AC_INIT([ngdevkit-examples], [0.1])

# ngdevkit-toolkit and ngdevkit can be installed in
# two different location, so search for executable
# in two steps

# ngdevkit-toolkit common tools
AC_PATH_PROG([nggcc], [m68k-neogeo-elf-gcc])
AC_PATH_PROG([ngas], [m68k-neogeo-elf-as])
AC_PATH_PROG([ngld], [m68k-neogeo-elf-ld])
AC_PATH_PROG([ngar], [m68k-neogeo-elf-ar])
AC_PATH_PROG([ngranlib], [m68k-neogeo-elf-ranlib])
AC_PATH_PROG([ngobjcopy], [m68k-neogeo-elf-objcopy])
AC_PATH_PROG([ngsdcc], [z80-neogeo-ihx-sdcc])
AC_PATH_PROG([ngsdas], [z80-neogeo-ihx-sdasz80])
AC_PATH_PROG([ngsdld], [z80-neogeo-ihx-sdldz80])
AC_PATH_PROG([ngsdar], [z80-neogeo-ihx-sdar])
AC_PATH_PROG([ngsdranlib], [z80-neogeo-ihx-sdranlib])
AC_PATH_PROG([ngsdobjcopy], [z80-neogeo-ihx-sdobjcopy])

# ngdevkit location and common flags
PKG_CHECK_MODULES([ngdevkit],[ngdevkit])
AC_SUBST([ngdkbindir],`pkg-config --variable=bindir ngdevkit`)
AC_SUBST([ngdksharedir],`pkg-config --variable=sharedir ngdevkit`)

# additional dependencies
AM_PATH_PYTHON([3.1])
AC_PATH_PROG([zip], [zip])
AC_PATH_PROG([convert], [convert])
AC_PATH_PROG([gngeo], [gngeo])

AC_CONFIG_FILES([
 Makefile.config
])
AC_OUTPUT
