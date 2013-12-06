#  Copyright 2005 Adam Jackson.
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  on the rights to use, copy, modify, merge, publish, distribute, sub
#  license, and/or sell copies of the Software, and to permit persons to whom
#  the Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice (including the next
#  paragraph) shall be included in all copies or substantial portions of the
#  Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
#  ADAM JACKSON BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

EPOXY = $(builddir)/../src/libepoxy.la

check_LTLIBRARIES = \
	$(EGL_LIBS) \
	$(GLX_LIBS) \
	$()

libegl_common_la_SOURCES = \
	egl_common.c \
	egl_common.h
	$()

libglx_common_la_SOURCES = \
	glx_common.c \
	glx_common.h
	$()

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(X11_CFLAGS)

TESTS = \
	$(EGL_TESTS) \
	$(GLX_TESTS) \
	headerguards \
	miscdefines \
	$()

check_PROGRAMS = $(TESTS)

if BUILD_EGL
EGL_TESTS = \
	egl_has_extension_nocontext \
	$()

EGL_LIBS = libegl_common.la
endif

if BUILD_GLX
GLX_TESTS = \
	glx_beginend \
	glx_public_api \
	glx_public_api_core \
	glx_glxgetprocaddress_nocontext \
	glx_has_extension_nocontext \
	$()

GLX_LIBS = libglx_common.la
endif

egl_has_extension_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libegl_common.la
egl_has_extension_nocontext_DEPENDENCIES = libegl_common.la

glx_beginend_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la $(GL_LIBS)
glx_beginend_DEPENDENCIES = libglx_common.la

glx_public_api_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_public_api_DEPENDENCIES = libglx_common.la

glx_public_api_core_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_public_api_core_DEPENDENCIES = libglx_common.la

glx_glxgetprocaddress_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_glxgetprocaddress_nocontext_DEPENDENCIES = libglx_common.la

glx_has_extension_nocontext_LDFLAGS = $(X11_LIBS) $(EPOXY) libglx_common.la
glx_has_extension_nocontext_DEPENDENCIES = libglx_common.la
