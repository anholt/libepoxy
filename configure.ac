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
        [1.5.4],
        [https://github.com/anholt/libepoxy],
        [libepoxy])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign -Wno-portability dist-xz no-dist-gzip tar-ustar subdir-objects])

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

AC_CHECK_HEADER([KHR/khrplatform.h],
                [AC_DEFINE([HAVE_KHRPLATFORM_H], [1],
                           [Define to 1 if you have <KHR/khrplatform.h> (used for tests)]
                           )]
               )

# OS X defaults to having -Wint-conversion ("warn when passing
# uintptr_t to a void *") by default.  Kill that.
XORG_TESTSET_CFLAG(CWARNFLAGS, [-Wno-int-conversion])

AC_ARG_ENABLE([x11],
              [AC_HELP_STRING([--enable-x11=@<:@yes,no@:>@], [Enable X11 support @<:@default=yes@:>@])],
              [enable_x11=$enableval],
              [enable_x11=yes])

AC_ARG_ENABLE([glx],
              [AC_HELP_STRING([--enable-glx=@<:@auto,yes,no@:>@], [Enable GLX support @<:@default=auto@:>@])],
              [enable_glx=$enableval],
              [enable_glx=auto])

# GLX can be used on different platforms, so we expose a
# configure time switch to enable or disable it; in case
# the "auto" default value is set, we only enable GLX
# support on Linux and Unix
AS_CASE([$enable_glx],
  [auto], [
    AS_CASE([$host_os],
            [mingw*], [build_glx=no],
            [darwin*], [build_glx=no],
            [android*], [build_glx=no],
            [build_glx=yes])
  ],

  [yes], [
    build_glx=yes
  ],

  [no], [
    build_glx=no
  ],

  [AC_MSG_ERROR([Invalid value "$enable_glx" for option "--enable-glx"])]
])

AC_ARG_ENABLE([egl],
              [AC_HELP_STRING([--enable-egl=@<:@auto,yes,no@:>@], [Enable EGL support @<:@default=auto@:>@])],
              [enable_egl=$enableval],
              [enable_egl=auto])

AS_CASE([$enable_egl],
  [auto], [
    AS_CASE([$host_os],
            [mingw*], [build_egl=no],
            [darwin*], [build_egl=no],
            [build_egl=yes])
  ],

  [yes], [
    build_egl=yes
  ],

  [no], [
    build_egl=no
  ],

  [AC_MSG_ERROR([Invalid value "$enable_egl" for option "--enable-egl"])]
])

# The remaining platform specific API are enabled depending on the
# platform we're building for
AS_CASE([$host_os],
  [mingw*], [
    build_wgl=yes
    has_znow=yes
    # On windows, the DLL has to have all of its functions
    # resolved at link time, so we have to link directly against
    # opengl32.dll.  But that's the only GL provider, anyway.
    EPOXY_LINK_LIBS="-lopengl32"

    # Testing our built windows binaries requires that they be run
    # under wine.  Yeah, we should be nice and autodetect, but
    # there's lots of missing autodetection for the testsuite
    # (like checking for EGL and GLX libs in non-windows.).
    AC_SUBST([LOG_COMPILER], [wine])
  ],

  [darwin*], [
    build_wgl=no
    has_znow=no
    EPOXY_LINK_LIBS=""
  ],

  [
    build_wgl=no
    has_znow=yes
    # On platforms with dlopen, we load everything dynamically and
    # don't link against a specific window system or GL implementation.
    EPOXY_LINK_LIBS=""
   ]
)

AC_SUBST(EPOXY_LINK_LIBS)

if test x$enable_x11 = xno; then
    if test x$enable_glx = xyes; then
        AC_MSG_ERROR([GLX support is explicitly enabled, but X11 was disabled])
    fi
    build_glx=no
else
    AC_DEFINE([ENABLE_X11], [1], [Whether X11 support is enabled])
fi

AM_CONDITIONAL(BUILD_EGL, test x$build_egl = xyes)
if test x$build_egl = xyes; then
    PKG_CHECK_MODULES(EGL, [egl])
    AC_DEFINE([BUILD_EGL], [1], [build EGL tests])
    AC_DEFINE(ENABLE_EGL, [1], [Whether EGL support is enabled])
fi

AM_CONDITIONAL(BUILD_GLX, test x$build_glx = xyes)
if test x$build_glx = xyes; then
    AC_DEFINE([BUILD_GLX], [1], [build GLX tests])
fi

AM_CONDITIONAL(BUILD_WGL, test x$build_wgl = xyes)
if test x$build_wgl = xyes; then
    AC_DEFINE([BUILD_WGL], [1], [build WGL tests])
fi

AM_CONDITIONAL(HAS_ZNOW, test x$has_znow = xyes)

AC_CHECK_LIB([GLESv1_CM], [glFlush], [has_gles1=yes], [has_gles1=no])
AM_CONDITIONAL(HAS_GLES1, test x$has_gles1 = xyes)

AC_CHECK_LIB([dl], [dlopen], [DLOPEN_LIBS="-ldl"])
AC_SUBST([DLOPEN_LIBS])

savelibs=$LIBS
LIBS=$DLOPEN_LIBS
AC_CHECK_FUNCS([dlvsym], [have_dlvsym=1], [have_dlvsym=0])
AM_CONDITIONAL(HAVE_DLVSYM, test $have_dlvsym = 1)
LIBS=$savelibs

VISIBILITY_CFLAGS=""
AS_CASE(["$host"],

  [*-*-mingw*], [
    dnl on mingw32 we do -fvisibility=hidden and __declspec(dllexport)
    AC_DEFINE([EPOXY_PUBLIC],
              [__attribute__((visibility("default"))) __declspec(dllexport) extern],
              [defines how to decorate public symbols while building])
    VISIBILITY_CFLAGS="-fvisibility=hidden"
  ],

  [
    dnl on other compilers, check if we can do -fvisibility=hidden
    SAVED_CFLAGS="${CFLAGS}"
    CFLAGS="-fvisibility=hidden"
    AC_MSG_CHECKING([for -fvisibility=hidden compiler flag])
    AC_TRY_COMPILE([], [int main (void) { return 0; }], [
      AC_MSG_RESULT(yes)
      enable_fvisibility_hidden=yes
    ], [
      AC_MSG_RESULT(no)
      enable_fvisibility_hidden=no
    ])
    CFLAGS="${SAVED_CFLAGS}"

    AS_IF([test "${enable_fvisibility_hidden}" = "yes"], [
      AC_DEFINE([EPOXY_PUBLIC],
                [__attribute__((visibility("default"))) extern],
                [defines how to decorate public symbols while building])
      VISIBILITY_CFLAGS="-fvisibility=hidden"
    ])
  ]
)

AC_SUBST([VISIBILITY_CFLAGS])

if test x$enable_x11 = xyes; then
    PKG_CHECK_MODULES(X11, [x11], [x11=yes], [x11=no])
    if test x$x11 = xno -a x$build_glx = xyes; then
        AC_MSG_ERROR([libX11 headers (libx11-dev) are required to build with GLX support])
    fi
else
    x11=no
fi

if test x$build_glx = xyes; then
  AC_DEFINE(ENABLE_GLX, [1], [Whether GLX support is enabled])
fi

AM_CONDITIONAL(HAVE_X11, test x$x11 = xyes)

PKG_CHECK_MODULES(GL, [gl], [gl=yes], [gl=no])
PKG_CHECK_MODULES(EGL, [egl], [egl=yes], [egl=no])

GL_REQS=""
AS_IF([test x$gl = xyes], [GL_REQS="$GL_REQS gl"])
AS_IF([test x$build_egl = xyes && test x$egl = xyes], [GL_REQS="$GL_REQS egl"])
AC_SUBST(GL_REQS)

# Variables for the pkg-config file; AC_SUBST does not do `test` substitutions,
# so we need to specify the boolean values here
AS_IF([test x$build_glx = xyes], [epoxy_has_glx=1], [epoxy_has_glx=0])
AS_IF([test x$build_egl = xyes], [epoxy_has_egl=1], [epoxy_has_egl=0])
AS_IF([test x$build_wgl = xyes], [epoxy_has_wgl=1], [epoxy_has_wgl=0])
AC_SUBST(epoxy_has_glx)
AC_SUBST(epoxy_has_egl)
AC_SUBST(epoxy_has_wgl)

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
echo "           WGL:         $build_wgl"
echo "        PYTHON:         $PYTHON"
