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

include Makefile.sources

EPOXY = $(builddir)/../src/libepoxy.la

check_LTLIBRARIES = \
	$(EGL_UTIL_LIB) \
	$(GLX_UTIL_LIB) \
	$(WGL_UTIL_LIB) \
	$()

libegl_common_la_SOURCES = \
	$(EGL_COMMON_SRC) \
	$()

libglx_common_la_SOURCES = \
	$(GLX_COMMON_SRC) \
	$()

libwgl_common_la_SOURCES = \
	$(WGL_COMMON_SRC) \
	$()

libwgl_common_la_LIBADD = $(EPOXY)

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(X11_CFLAGS) \
	$(EGL_CFLAGS) \
	$()

AM_CFLAGS = $(CWARNFLAGS)

TESTS = \
	$(EGL_TESTS) \
	$(GLX_TESTS) \
	$(EGL_AND_GLX_TESTS) \
	$(WGL_TESTS) \
	$(GENERIC_TEST_PROGS) \
	$()

check_BINARIES = $(EGL_AND_GLX_BIN)

XFAIL_TESTS = \
        egl_and_glx_different_pointers_egl_glx \
	$()

check_PROGRAMS = $(TESTS)

if BUILD_EGL
EGL_TESTS = \
	$(EGL_TEST_PROGS) \
	$()

EGL_UTIL_LIB = libegl_common.la
endif

if BUILD_GLX
if HAS_ZNOW
GLX_SHARED_ZNOW = glx_shared_znow
endif

if BUILD_EGL
if BUILD_GLX
if !BUILD_APPLE
EGL_AND_GLX_TESTS = \
	$(EGL_GLX_TEST_PROGS) \
        $()
endif
endif
endif

if !BUILD_APPLE
GLX_NON_APPLE_TESTS = \
	$(GLX_NON_APPLE_PROGS) \
        $()
endif

GLX_TESTS = \
	$(GLX_TEST_PROGS) \
        $(GLX_SHARED_ZNOW) \
        $(GLX_NON_APPLE_TESTS) \
	$()

GLX_UTIL_LIB = libglx_common.la
endif

if BUILD_WGL
WGL_TESTS = \
	$(WGL_TEST_PROGS) \
	$()

WGL_UTIL_LIB = libwgl_common.la
endif

egl_has_extension_nocontext_LDADD = $(EPOXY) libegl_common.la $(X11_LIBS)

egl_gl_LDADD = $(EPOXY) $(DLOPEN_LIBS) libegl_common.la $(X11_LIBS)

egl_gles1_without_glx_CPPFLAGS = $(AM_CPPFLAGS) -DGLES_VERSION=1
egl_gles1_without_glx_SOURCES = egl_without_glx.c
egl_gles1_without_glx_LDADD = $(EPOXY) $(DLOPEN_LIBS) libegl_common.la $(X11_LIBS)

egl_gles2_without_glx_CPPFLAGS = $(AM_CPPFLAGS) -DGLES_VERSION=2
egl_gles2_without_glx_SOURCES = egl_without_glx.c
egl_gles2_without_glx_LDADD = $(EPOXY) $(DLOPEN_LIBS) libegl_common.la $(X11_LIBS)

egl_and_glx_different_pointers_egl_SOURCES = egl_and_glx_different_pointers.c $(DLWRAP_SRC)
egl_and_glx_different_pointers_egl_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_egl_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_egl_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_EGL

egl_and_glx_different_pointers_glx_SOURCES = egl_and_glx_different_pointers.c $(DLWRAP_SRC)
egl_and_glx_different_pointers_glx_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_glx_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_glx_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLX

egl_and_glx_different_pointers_egl_glx_SOURCES = egl_and_glx_different_pointers.c $(DLWRAP_SRC)
egl_and_glx_different_pointers_egl_glx_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_egl_glx_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_egl_glx_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_EGL -DUSE_GLX

glx_alias_prefer_same_name_SOURCES = glx_gles2.c $(DLWRAP_SRC)
glx_alias_prefer_same_name_LDFLAGS = -rdynamic
glx_alias_prefer_same_name_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS) -ldl

glx_beginend_LDADD = $(EPOXY) libglx_common.la $(GL_LIBS) $(X11_LIBS)

glx_gles2_SOURCES = glx_gles2.c $(DLWRAP_SRC)
glx_gles2_LDFLAGS = -rdynamic
glx_gles2_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS) -ldl

glx_public_api_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS)

glx_public_api_core_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS)

glx_glxgetprocaddress_nocontext_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS)

glx_has_extension_nocontext_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS)

glx_static_CFLAGS = -DNEEDS_TO_BE_STATIC
glx_static_LDADD = $(DLOPEN_LIBS) $(EPOXY) libglx_common.la $(X11_LIBS)
glx_static_LDFLAGS = -static

glx_shared_znow_SOURCES = glx_static.c
glx_shared_znow_LDADD = $(DLOPEN_LIBS) $(EPOXY) libglx_common.la $(X11_LIBS)
glx_shared_znow_LDFLAGS = -Wl,-z,now

khronos_typedefs_SOURCES = \
	$(KHRONOS_TYPEDEF_SRC) \
	$()

wgl_core_and_exts_LDADD = $(EPOXY) libwgl_common.la -lgdi32

wgl_per_context_funcptrs_LDADD = $(EPOXY) libwgl_common.la -lgdi32

wgl_usefontbitmaps_LDADD = $(EPOXY) libwgl_common.la -lgdi32
wgl_usefontbitmaps_unicode_SOURCES = wgl_usefontbitmaps.c
wgl_usefontbitmaps_unicode_LDADD = $(EPOXY) libwgl_common.la -lgdi32
wgl_usefontbitmaps_unicode_CPPFLAGS = $(AM_CPPFLAGS) -DUNICODE

EXTRA_DIST = Makefile.vc
