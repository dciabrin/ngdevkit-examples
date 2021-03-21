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

dnl
dnl MinGW-specific options
dnl
AC_ARG_ENABLE([mingw],
	      AS_HELP_STRING([--enable-mingw], [Look for a Win10-native version of gngeo available in PATH]),
	      AS_IF([test "x$enable_mingw" = xyes], [ENABLE_MINGW="yes"], [ENABLE_MINGW="no"]),
              [ENABLE_MINGW="no"])
AC_SUBST([ENABLE_MINGW])
AC_ARG_ENABLE([msys2],
              AS_HELP_STRING([--enable-msys2], [Look for a Win10-native version of gngeo available in PATH]),
              AS_IF([test "x$enable_msys2" = xyes], [ENABLE_MSYS2="yes"], [ENABLE_MSYS2="no"]),
              [ENABLE_MSYS2="no"])
AC_SUBST([ENABLE_MSYS2])

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
AC_ARG_WITH([python],
            AS_HELP_STRING([--with-python=path], [Absolute path to the python 3 binary]))

if test -n "$with_python"; then
  python="$with_python"
  AC_SUBST([python])
else
  AC_PATH_PROGS([python], [python3 python], [no])
  if test x"$python" = x"no"; then
    AC_MSG_ERROR([no python detected.])
  fi
fi

dnl AC_PATH_PROGS([python], [python3 python], [no])
dnl if test x"$python" = x"no"; then
dnl   AC_MSG_ERROR([no python detected.])
dnl fi
AC_MSG_CHECKING([whether detected python is at least python3])
[pyver=`$python -c 'import sys; print(sys.version_info[0])'`]
if test $pyver -ge 3; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_ERROR([python major version too old (detected: $pyver, required: 3)])
fi

dnl Configure how to run ngdevkit's python tools
AC_MSG_CHECKING([how to call ngdevkit's python tools])
ngdktool=$ngdkbindir/paltool.py
shebang=`head -1 $ngdktool | sed -ne 's|^#!\(.*python\S*\)\s*.*|\1|p'`
if test -n "$shebang" ; then
    ngdkpython=`which $shebang | grep -i python`
else
    ngdkpython=""
fi
if test -f "$ngdkpython" && test "$ngdkpython" = "$python" ; then
  PALTOOL="$ngdkbindir/paltool.py"
  TILETOOL="$ngdkbindir/tiletool.py"
  ADPCMTOOL="$ngdkbindir/adpcmtool.py"
  AC_MSG_RESULT([run directly])
else
  PALTOOL="$python $ngdkbindir/paltool.py"
  TILETOOL="$python $ngdkbindir/tiletool.py"
  ADPCMTOOL="$python $ngdkbindir/adpcmtool.py"
  AC_MSG_RESULT([run with $python])
fi
AC_SUBST([PALTOOL])
AC_SUBST([TILETOOL])
AC_SUBST([ADPCMTOOL])

dnl External tools for the examples
AC_PATH_PROG([zip], [zip])
AC_PATH_PROG([convert], [convert])
AC_PATH_PROG([sox], [sox])
AC_PATH_PROG([rsync], [rsync])

dnl Right now MSYS2 is a synonymous for MINGW
if test x"$ENABLE_MSYS2" = x"yes"; then
  ENABLE_MINGW=yes
fi
if test x"$ENABLE_MINGW" = x"yes"; then
gngeo_name=ngdevkit-gngeo.exe
else
gngeo_name=ngdevkit-gngeo
fi

AC_PATH_PROG([gngeo], [$gngeo_name])
# escape potential spaces from path
gngeo=$(echo "$gngeo" | sed 's/ /\\ /g')

if test x"$ENABLE_MINGW" = x"yes"; then
  GNGEO_ABS_PREFIX="`eval dirname ${gngeo}`/"
  GNGEO_PRE="cd \"$GNGEO_ABS_PREFIX\" &&"
  GNGEO_POST="; cat gngeo_log.txt"
else
  GNGEO_ABS_PREFIX=""
  GNGEO_PRE=""
  GNGEO_POST=""
fi

if test x"$gngeo" != x""; then
  if test x"$ENABLE_MINGW" = x"yes"; then
    AC_MSG_CHECKING([ngdevkit-gngeo install path])
    gngeo_install_path="`eval dirname ${gngeo}`"
    if eval test -d \"${gngeo_install_path}\"; then
      AC_MSG_RESULT([$gngeo_install_path])
      GNGEO_INSTALL_PATH=$(echo "$gngeo_install_path" | sed 's/ /\\ /g')
    else
      AC_MSG_ERROR([not found])
    fi
    AC_SUBST([GNGEO_INSTALL_PATH])
  fi

  AC_MSG_CHECKING([whether ngdevkit-gngeo is built with GLSL shader blitter])
  (eval $GNGEO_PRE $gngeo -b help $GNGEO_POST) | grep -q -w '^glsl'
  if test $? = 0; then
    AC_MSG_RESULT([yes])
    GNGEO_GLSL=yes
  else
    GNGEO_GLSL=no
    AC_MSG_RESULT([no])
  fi
fi
AC_SUBST([GNGEO_GLSL])

if test x"$GNGEO_GLSL" = x"yes"; then
AC_SUBST([GNGEO_SHADER_PATH])

if test x"$GNGEO_GLSL" = x"yes"; then
  AC_MSG_CHECKING([ngdevkit-gngeo GLSL shader path])
  gngeo_shader_path=`(eval $GNGEO_PRE $gngeo -b glsl --shader=noop.glslp forcefail $GNGEO_POST) | sed -ne 's%.*GLSL preset \(.*\)/noop\.glslp.*%\1%p'`
  dnl MinGW generates self-contained builds, so all paths are relative
  if test -n "$gngeo_shader_path" && test -d "${GNGEO_ABS_PREFIX}$gngeo_shader_path"; then
    AC_MSG_RESULT([$gngeo_shader_path])
    GNGEO_SHADER_PATH=$gngeo_shader_path
  else
    AC_MSG_RESULT([not found])
  fi

  AC_MSG_CHECKING([whether the GLSL shaders directory is available])
    if test -d "$PWD/shaders"; then
      AC_MSG_RESULT([$PWD/shaders])
      GLSL_SHADER_PATH=$PWD/shaders
    else
      AC_MSG_RESULT([not found. git submodule not checked out])
    fi
  fi
fi
AC_SUBST([GNGEO_SHADER_PATH])
AC_SUBST([GLSL_SHADER_PATH])

AC_MSG_CHECKING([which default shader to use in ngdevkit-gngeo])
if test -n "$GLSL_SHADER_PATH" && test -f "$GLSL_SHADER_PATH/qcrt-flat.glslp"; then
  SHADER_PATH=$GLSL_SHADER_PATH
  SHADER=qcrt-flat.glslp
else
  SHADER_PATH=$GNGEO_SHADER_PATH
  SHADER=noop.glslp
fi
AC_MSG_RESULT([$SHADER_PATH/$SHADER])
AC_SUBST([SHADER_PATH])
AC_SUBST([SHADER])


AC_CONFIG_FILES([
 Makefile.config
])
AC_OUTPUT
