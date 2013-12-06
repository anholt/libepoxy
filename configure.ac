# Copyright © 2013 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([libepoxy],
        [1.0],
        [https://people.freedesktop.org/~anholt/libepoxy],
        [libepoxy])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2 subdir-objects])

# Require X.Org macros 1.8 or later for MAN_SUBSTS set by XORG_MANPAGE_SECTIONS
m4_ifndef([XORG_MACROS_VERSION],
          [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen.
  Hint: either install from source, git://anongit.freedesktop.org/xorg/util/macros or,
  depending on you distribution, try package 'xutils-dev' or 'xorg-x11-util-macros'])])

XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS

AC_CHECK_PROGS([PYTHON], [python3 python2 python])

# Initialize libtool
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SYS_LARGEFILE

case $host_os in
    mingw*)
        build_egl=no
        build_glx=no
        # On windows, the DLL has to have all of its functions
        # resolved at link time, so we have to link directly aginst
        # opengl32.dll.  But that's the only GL provider, anyway.
        EPOXY_LINK_LIBS="-lopengl32"
        ;;
    *)
        build_egl=yes
        build_glx=yes
        # On platforms with dlopen, we load everything dynamically and
        # don't link against a specific window system or GL implementation.
        EPOXY_LINK_LIBS=""
        ;;
esac

AC_SUBST(EPOXY_LINK_LIBS)

AM_CONDITIONAL(BUILD_EGL, test x$build_egl = xyes)
if test x$build_egl = xyes; then
    AC_DEFINE([BUILD_EGL], [1], [build EGL tests])
fi

AM_CONDITIONAL(BUILD_GLX, test x$build_glx = xyes)
if test x$build_glx = xyes; then
    AC_DEFINE([BUILD_GLX], [1], [build GLX tests])
fi


case $host_os in
    mingw*)
        # visibility flags aren't supported for windows DLLs, and the
        # compiler whines to tell you so, so don't set them up.
        ;;
    *)
        if test "x$GCC" = xyes; then
            save_CFLAGS="$CFLAGS"
            AC_MSG_CHECKING([whether $CC supports -fvisibility=hidden])
            VISIBILITY_CFLAGS="-fvisibility=hidden"
            CFLAGS="$CFLAGS $VISIBILITY_CFLAGS"
            AC_LINK_IFELSE([AC_LANG_PROGRAM()], AC_MSG_RESULT([yes]),
                           [VISIBILITY_CFLAGS=""; AC_MSG_RESULT([no])]);

            # Restore CFLAGS; VISIBILITY_CFLAGS are added to it where needed.
            CFLAGS=$save_CFLAGS
        fi
        ;;
esac

AC_SUBST([VISIBILITY_CFLAGS])

PKG_CHECK_MODULES(X11, [x11], [x11=yes], [x11=no])
AM_CONDITIONAL(HAVE_X11, test x$x11 = xyes)

PKG_CHECK_MODULES(GL, [gl], [gl=yes], [gl=no])

AC_CONFIG_FILES([
                epoxy.pc
                Makefile
                include/epoxy/Makefile
                src/Makefile
                test/Makefile
])
AC_OUTPUT

echo "           EGL:         $build_egl"
echo "           GLX:         $build_glx"
echo "        PYTHON:         $PYTHON"
