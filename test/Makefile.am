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
	$(EGL_LIBS) \
	$(GLX_LIBS) \
	$(WGL_LIBS) \
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
	$(X11_CFLAGS)

TESTS = \
	$(EGL_TESTS) \
	$(GLX_TESTS) \
	$(WGL_TESTS) \
	headerguards$(EXEEXT) \
	miscdefines$(EXEEXT) \
	$()

check_PROGRAMS = $(TESTS)

if BUILD_EGL
EGL_TESTS = \
	egl_has_extension_nocontext \
	$()

EGL_LIBS = libegl_common.la
endif

if BUILD_GLX
if HAS_ZNOW
GLX_SHARED_ZNOW = glx_shared_znow
endif

GLX_TESTS = \
	glx_beginend \
	glx_public_api \
	glx_public_api_core \
	glx_glxgetprocaddress_nocontext \
	glx_has_extension_nocontext \
	glx_static \
        $(GLX_SHARED_ZNOW) \
	$()

GLX_LIBS = libglx_common.la
endif

if BUILD_WGL
WGL_TESTS = \
	wgl_core_and_exts$(EXEEXT) \
	$()

WGL_LIBS = libwgl_common.la
endif

egl_has_extension_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libegl_common.la
egl_has_extension_nocontext_DEPENDENCIES = $(EPOXY) libegl_common.la

glx_beginend_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la $(GL_LIBS)
glx_beginend_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_public_api_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_public_api_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_public_api_core_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_public_api_core_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_glxgetprocaddress_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_glxgetprocaddress_nocontext_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_has_extension_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_has_extension_nocontext_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_static_CFLAGS = -DNEEDS_TO_BE_STATIC
glx_static_LDFLAGS = -ldl $(X11_LIBS) -static $(EPOXY) libglx_common.la
glx_static_DEPENDENCIES = $(EPOXY) libglx_common.la

glx_shared_znow_SOURCES = glx_static.c
glx_shared_znow_LDFLAGS = -ldl $(X11_LIBS) -z now $(EPOXY) libglx_common.la
glx_shared_znow_DEPENDENCIES = $(EPOXY) libglx_common.la

wgl_core_and_exts_LDFLAGS = $(EPOXY) libwgl_common.la -lgdi32
wgl_core_and_exts_DEPENDENCIES = $(EPOXY) libwgl_common.la
