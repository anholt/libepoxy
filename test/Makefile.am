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

EPOXY = $(builddir)/../src/libepoxy.la

check_LTLIBRARIES = \
	$(EGL_UTIL_LIB) \
	$(GLX_UTIL_LIB) \
	$(WGL_UTIL_LIB) \
	$()

libegl_common_la_SOURCES = \
	egl_common.c \
	egl_common.h
	$()

libglx_common_la_SOURCES = \
	glx_common.c \
	glx_common.h
	$()

libwgl_common_la_SOURCES = \
	wgl_common.c \
	wgl_common.h
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
	headerguards$(EXEEXT) \
	miscdefines$(EXEEXT) \
	khronos_typedefs$(EXEEXT) \
	$()

check_BINARIES = $(EGL_AND_GLX_BIN)

XFAIL_TESTS = \
        egl_and_glx_different_pointers_egl_glx \
	$()

check_PROGRAMS = $(TESTS)

if BUILD_EGL
EGL_TESTS = \
	egl_has_extension_nocontext \
	egl_gl \
	egl_gles1_without_glx \
	egl_gles2_without_glx \
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
	egl_and_glx_different_pointers_egl_glx \
	egl_and_glx_different_pointers_egl \
	egl_and_glx_different_pointers_glx \
        $()
endif
endif
endif

if !BUILD_APPLE
GLX_NON_APPLE_TESTS = \
	glx_alias_prefer_same_name \
	glx_gles2 \
        $()
endif

GLX_TESTS = \
	glx_beginend \
	glx_public_api \
	glx_public_api_core \
	glx_glxgetprocaddress_nocontext \
	glx_has_extension_nocontext \
	glx_static \
        $(GLX_SHARED_ZNOW) \
        $(GLX_NON_APPLE_TESTS) \
	$()

GLX_UTIL_LIB = libglx_common.la
endif

if BUILD_WGL
WGL_TESTS = \
	wgl_core_and_exts$(EXEEXT) \
	wgl_per_context_funcptrs$(EXEEXT) \
	wgl_usefontbitmaps$(EXEEXT) \
	wgl_usefontbitmaps_unicode$(EXEEXT) \
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

egl_and_glx_different_pointers_egl_SOURCES = egl_and_glx_different_pointers.c dlwrap.c dlwrap.h
egl_and_glx_different_pointers_egl_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_egl_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_egl_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_EGL

egl_and_glx_different_pointers_glx_SOURCES = egl_and_glx_different_pointers.c dlwrap.c dlwrap.h
egl_and_glx_different_pointers_glx_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_glx_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_glx_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLX

egl_and_glx_different_pointers_egl_glx_SOURCES = egl_and_glx_different_pointers.c dlwrap.c dlwrap.h
egl_and_glx_different_pointers_egl_glx_LDADD = libegl_common.la libglx_common.la $(DLOPEN_LIBS) $(EPOXY) $(X11_LIBS)
egl_and_glx_different_pointers_egl_glx_LDFLAGS = -rdynamic
egl_and_glx_different_pointers_egl_glx_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_EGL -DUSE_GLX

glx_alias_prefer_same_name_SOURCES = glx_gles2.c dlwrap.c dlwrap.h
glx_alias_prefer_same_name_LDFLAGS = -rdynamic
glx_alias_prefer_same_name_LDADD = $(EPOXY) libglx_common.la $(X11_LIBS) -ldl

glx_beginend_LDADD = $(EPOXY) libglx_common.la $(GL_LIBS) $(X11_LIBS)

glx_gles2_SOURCES = glx_gles2.c dlwrap.c dlwrap.h
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
	khronos_typedefs.c \
	khronos_typedefs.h \
	khronos_typedefs_nonepoxy.c \
	$()

wgl_core_and_exts_LDADD = $(EPOXY) libwgl_common.la -lgdi32

wgl_per_context_funcptrs_LDADD = $(EPOXY) libwgl_common.la -lgdi32

wgl_usefontbitmaps_LDADD = $(EPOXY) libwgl_common.la -lgdi32
wgl_usefontbitmaps_unicode_SOURCES = wgl_usefontbitmaps.c
wgl_usefontbitmaps_unicode_LDADD = $(EPOXY) libwgl_common.la -lgdi32
wgl_usefontbitmaps_unicode_CPPFLAGS = $(AM_CPPFLAGS) -DUNICODE
