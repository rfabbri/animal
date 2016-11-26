# Process this file with autoconf to produce a configure script.
#
# $Revision: 1.8 $ $Date: 2010-04-19 20:15:46 $
# Copyright (C) 2003 Ricardo Fabbri
# Some inspiration came from ImageMagick's configure.ac (thx Bob F.)
# and examples from the Autobook (http://sources.redhat.com/autobook)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# TODO
# --with-imagemagick-prefix not working 
#

AC_PREREQ(2.61)
define([MAJOR_VERSION_M4],[[0]])dnl
define([MINOR_VERSION_M4],[[16]])dnl
define([MICRO_VERSION_M4],[[6]])dnl
AC_INIT([animal],MAJOR_VERSION_M4.MINOR_VERSION_M4.MICRO_VERSION_M4,
   [ricardofabbri@users.sourceforge.net])
echo "configuring AnImaL MAJOR_VERSION_M4.MINOR_VERSION_M4.MICRO_VERSION_M4"
MAJOR_VERSION=MAJOR_VERSION_M4
MINOR_VERSION=MINOR_VERSION_M4
MICRO_VERSION=MICRO_VERSION_M4
AC_SUBST(MAJOR_VERSION)dnl
AC_SUBST(MINOR_VERSION)dnl
AC_SUBST(MICRO_VERSION)dnl

AC_CONFIG_AUX_DIR([config])
AC_CANONICAL_HOST()
AC_CONFIG_SRCDIR([animal/animal.h])
AC_CONFIG_HEADER([animal/config.h])
AX_PREFIX_CONFIG_H([animal/animal-config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
if test x"$ac_cv_prog_cc_stdc" = x'no'; then
   AC_MSG_WARN([[found the $CC compiler but it is not ANSI-C compliant.]])
fi

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Check to see if building shared and static libraries
libtool_build_shared_libs='no'
if ./libtool --config | grep 'build_libtool_libs=yes' > /dev/null
then
  libtool_build_shared_libs='yes'
fi

libtool_build_static_libs='no'
if ./libtool --config | grep 'build_old_libs=yes' > /dev/null
then
  libtool_build_static_libs='yes'
fi

#
# Tests for windoze
# 
native_win32_build='no'
cygwin_build='no'
case $host_os in
  cygwin*) 
      cygwin_build='yes'
      ;;
  mingw* )
      native_win32_build='yes' 
      ;;
esac

USERLIBS=$LIBS

#
# Set defines required to build DLLs and modules using MinGW
# thx to Bob F.
#
# These options are set for DLL build
#  libanimal:           _DLL ANIMAL_LIB
#
LIBRARY_EXTRA_CPPFLAGS=''
if test "${native_win32_build}" = 'yes'
then
  if test "${libtool_build_shared_libs}" = 'yes'
  then
    CPPFLAGS="$CPPFLAGS -D_DLL"
    LIBRARY_EXTRA_CPPFLAGS="-DANIMAL_LIB"
  else
    CPPFLAGS="$CPPFLAGS -D_LIB"
  fi
fi
AC_SUBST(LIBRARY_EXTRA_CPPFLAGS)


# Checks for libraries.


# Find math library
LIB_MATH=''
AC_CHECK_LIB(m,sqrt,LIB_MATH="-lm",,)
LIBS="$LIB_MATH $LIBS"

if test "${native_win32_build}" = 'yes'
then
   AC_MSG_CHECKING([ImageMagick location])
   if test -z "$MAGICK_DIR"; then
      MAGICK_DIR="/c/imagemagick"
   fi
   AC_MSG_RESULT([assuming $MAGICK_DIR])

   LIBS="$LIBS -lCORE_RL_magick_"
   LDFLAGS="$LDFLAGS -L$MAGICK_DIR"
   CPPFLAGS="$CPPFLAGS -W -Wall -I$MAGICK_DIR/include"
   # CPPFLAGS="$CPPFLAGS -D_MAGICKLIB_"
else
#
# Checks for ImageMagick using generic macros 
# See acinclude.m4
#
AC_PATH_LIB_LIBCONFIG

have_magick='no'
AC_PATH_LIB([ImageMagick], [[5.4.2 -nocheck]], [Magick-config], ,[
   # if found:
   if test x$IMAGEMAGICK_VERSION = "x5.5.5"; then
      AC_MSG_ERROR([[ImageMagick 5.5.5 has a bug which make it unusable with 
AnImaL. Please download and install any other version from www.imagemagick.org]])
   fi
   if test x$IMAGEMAGICK_VERSION = "x6.0.1" -o x$IMAGEMAGICK_VERSION = "x6.0.0" ; then
      AC_MSG_WARN([[
****************
ImageMagick 6.0.0 and 6.0.1 have a bug which can \
make some images not to be read. To have a fully-functional \
system please download and install another version from \
www.imagemagick.org
****************]])
   fi
   MAGICK_CPPFLAGS="`Magick-config --cppflags`"
   MAGICK_LDFLAGS="`Magick-config --ldflags`"
   MAGICK_LIBS="`Magick-config --libs`"

   AC_SUBST(MAGICK_LIBS)
   AC_SUBST(MAGICK_LDFLAGS)
   AC_SUBST(MAGICK_CPPFLAGS)

   LIBS="$MAGICK_LIBS $LIBS"
   LDFLAGS="$MAGICK_LDFLAGS $LDFLAGS"
   CPPFLAGS="$MAGICK_CPPFLAGS $CPPFLAGS"
   have_magick=yes
], [
    AC_MSG_RESULT([[First try to configure ImageMagick version >= 5.4.2 could not be done by Magick-config, 
trying pkg-config]])
    have_magick=no
],
[--cppflags --cflags], [--ldflags --libs])
dnl echo =============
dnl echo Magick: $have_magick
dnl echo =============

if test "$have_magick" = 'no'; then
  # Try finding imagemagick by pkg-config
  PKG_CHECK_MODULES(MAGICK, 
      [MagickCore >= 5.4.2],
      [CFLAGS="$CFLAGS $MAGICK_CFLAGS"
       LDFLAGS="$LDFLAGS $MAGICK_LDFLAGS"
       CPPFLAGS="$CPPFLAGS $MAGICK_CPPFLAGS"
       LIBS="$LIBS $MAGICK_LIBS"
       have_magick=yes
       # On older versions of pkg-config, the PKG_* macro will not call AC_SUBST on
       # these variables; modern versions (at least version 0.24) will take care
       # of that already. Running it twice, though, will not cause problems...
       AC_SUBST([MAGICK_LIBS])
       AC_SUBST([MAGICK_LDFLAGS])
       AC_SUBST([MAGICK_CPPFLAGS])
       AC_DEFINE(HAVE_MAGICK,[], [define HAVE_MAGICK if you have ImageMagick])],
      [AC_MSG_ERROR([[ ImageMagick >= 5.4.2 not found ]])])
fi

#   END of ImageMagick tests
dnl ===========================================================================





AC_CHECK_LIB([Magick],[GetLocaleExceptionMessage],[
  # if found
  AC_DEFINE([HAVE_GetLocaleExceptionMessage],[], [Define if \
ImageMagick has locale error messages.])
   ]
, [
  # if not found, try MagickCore
  AC_CHECK_LIB([MagickCore],[GetLocaleExceptionMessage],[
    AC_DEFINE([HAVE_GetLocaleExceptionMessage],[], [Define if \
  ImageMagick has locale error messages.])] , [])
])

fi # end if test "${native_win32_build}" = 'yes'

dnl AC_CHECK_LIB(gandalf,gan_err_report,[],[
dnl    AC_MSG_ERROR([[Gandalf library detection failed.]])])

# ---- Finished gandalf checking ----

AC_ARG_ENABLE(dev-build, [AC_HELP_STRING([--enable-dev-build],
   [define some flags useful for development])],
   [with_dev_build='yes'],
   [with_dev_build='no'])

# Debugging support.
AC_ARG_ENABLE([debug], 
  [AC_HELP_STRING([--enable-debug], [Do not optimize code by -NDEBUG flag (default disabled)])])

# Enable gprof-based profiling support
AC_ARG_ENABLE(gprof,[AC_HELP_STRING([--enable-gprof],
   [enable 'gprof' profiling support (default disabled)])],
   [with_gprof='yes'],
   [with_gprof='no'])

with_profiling='no'
if test "$with_gprof" = 'yes'
then
  with_profiling='yes'
  if test "$libtool_build_shared_libs" = 'yes'; then
    echo "Warning: Can not profile code using shared libraries"
  fi
fi

# P4 Optimization support.
AC_ARG_ENABLE(opt, [AC_HELP_STRING([--enable-opt=p4],
   [optimize for Penthium 4 (default disabled)])],
   [with_opt=$enableval],
   [with_opt='no'])

# Evaluate shell variable equivalents to Makefile directory variables
if test "x$prefix" = xNONE; then
   prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = xNONE; then
   exec_prefix=${prefix}
fi
PREFIX_DIR="$prefix"
eval "eval PREFIX_DIR=$prefix"
AC_SUBST(PREFIX_DIR)
eval "eval EXEC_PREFIX_DIR=$exec_prefix"
AC_SUBST(EXEC_PREFIX_DIR)
eval "eval LIB_DIR=$libdir"
AC_SUBST(LIB_DIR)
eval "eval INCLUDE_DIR=$includedir"
AC_SUBST(INCLUDE_DIR)

# Checks for header files.
AC_HEADER_STDC
if test x"$ac_cv_header_stdc" = x'no'; then
   AC_MSG_WARN([[this package uses ANSI-C headers but none were found.]])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL

# Check sizes of simple types
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)

# Checks for library functions.
TMPLIBS=$LIBS;
LIBS="$USERLIBS $LIB_MATH";
AC_CHECK_FUNCS([hypot memcpy]) 
# hypot is replaced in animal/common.h in case it isn't found
# memcpy is replaced in animal/base/misc_defs.{c,h} in case it isn't found

LIBS=$TMPLIBS;

#
# Handle special compiler flags
#
if test $with_dev_build = 'yes'; then
   case "$CC" in
      *gcc*)
        CFLAGS="$CFLAGS -pipe"
        CXXFLAGS="$CXXFLAGS -pipe"
      ;;
    esac
fi

if test "x$enable_debug" != 'xyes'; then
   dnl  CFLAGS="`echo "$CFLAGS"| sed "s/-g//g"`"
   CFLAGS="$CFLAGS -DNDEBUG"
   CXXFLAGS="$CXXFLAGS -DNDEBUG"
else 
   case "$CC" in
      *gcc*)
        # CFLAGS=["`echo "$CFLAGS"| sed "s/-O[0-9+]/-O0/g"`"]
        CFLAGS="$CFLAGS -W -Wall -Wno-implicit-int"
      ;;
    esac
fi

if test "x$with_opt" = xp4; then
   P4CFLAGS="-march=pentium4 -O3 -funroll-loops -pipe"
   CFLAGS="$P4CFLAGS"
   CXXFLAGS="$P4CFLAGS"
fi

# Add '-pg' if gprof source profiling support enabled
# Add '-a also??"
# Its more useful to call with -g flags (enable-debug; default)
if test "$with_gprof" = 'yes'
then
  CFLAGS="-pg $CFLAGS"
  CXXFLAGS="-pg $CXXFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

AC_CONFIG_FILES([Makefile animal/Makefile src/Makefile config/Makefile \
   config/animal.pc config/animal-config])
AC_OUTPUT

echo 
echo "Final Compilation Flags:"
echo "  CC       = $CC"
echo "  CFLAGS   = $CFLAGS"
echo "  CPPFLAGS = $CPPFLAGS"
echo "  LDFLAGS  = $LDFLAGS"
echo "  LIBS     = $LIBS"
echo

