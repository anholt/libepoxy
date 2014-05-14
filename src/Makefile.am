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

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$()

AM_CFLAGS = \
	$(CWARNFLAGS) \
	$(VISIBILITY_CFLAGS) \
	$(X11_CFLAGS) \
	$()

epoxyincludedir = $(includedir)/epoxy
lib_LTLIBRARIES = libepoxy.la

epoxyinclude_DATA = \
	$(GENERATED_GL_INCLUDES) \
	$(INSTALL_GLX_INCLUDES) \
	$(INSTALL_EGL_INCLUDES) \
	$(INSTALL_WGL_INCLUDES) \
	$()

if BUILD_EGL
INSTALL_EGL_INCLUDES = $(GENERATED_EGL_INCLUDES)
endif

if BUILD_GLX
INSTALL_GLX_INCLUDES = $(GENERATED_GLX_INCLUDES)
endif

if BUILD_WGL
INSTALL_WGL_INCLUDES = $(GENERATED_WGL_INCLUDES)
endif

GENERATED_GL_INCLUDES = \
	$(builddir)/../include/epoxy/gl_generated.h \
	$()

GENERATED_GLX_INCLUDES = \
	$(builddir)/../include/epoxy/glx_generated.h \
	$()

GENERATED_EGL_INCLUDES = \
	$(builddir)/../include/epoxy/egl_generated.h \
	$()

GENERATED_WGL_INCLUDES = \
	$(builddir)/../include/epoxy/wgl_generated.h \
	$()

GENERATED_GL_SOURCE = gl_generated_dispatch.c

GENERATED_GL = \
	$(GENERATED_GL_SOURCE) \
	$(GENERATED_GL_INCLUDES) \
	$()

GENERATED_GLX_SOURCE = glx_generated_dispatch.c

GENERATED_GLX = \
	$(GENERATED_GLX_SOURCE) \
	$(GENERATED_GLX_INCLUDES) \
	$()

GENERATED_EGL_SOURCE = egl_generated_dispatch.c

GENERATED_EGL = \
	$(GENERATED_EGL_SOURCE) \
	$(GENERATED_EGL_INCLUDES) \
	$()

GENERATED_WGL_SOURCE = wgl_generated_dispatch.c

GENERATED_WGL = \
	$(GENERATED_WGL_SOURCE) \
	$(GENERATED_WGL_INCLUDES) \
	$()

BUILT_SOURCES = \
	$(GENERATED_GL) \
	$(GENERATED_GLX) \
	$(GENERATED_EGL) \
	$(GENERATED_WGL) \
	$()
CLEANFILES = $(BUILT_SOURCES)

libepoxy_la_SOURCES = \
	dispatch_common.c \
	dispatch_common.h \
	$(GENERATED_GL) \
	$(BUILD_EGL_CODE) \
	$(BUILD_GLX_CODE) \
	$(BUILD_WGL_CODE) \
	$()

libepoxy_la_LDFLAGS = \
	-no-undefined \
        -Bsymbolic \
	$()

libepoxy_la_LIBADD = \
	$(EPOXY_LINK_LIBS) \
        $(DLOPEN_LIBS) \
	$()

if BUILD_EGL
BUILD_EGL_CODE = \
	$(GENERATED_EGL) \
	dispatch_egl.c \
	$()
endif

if BUILD_GLX
BUILD_GLX_CODE = \
	$(GENERATED_GLX) \
	dispatch_glx.c \
	$()
endif

if BUILD_WGL
BUILD_WGL_CODE = \
	$(GENERATED_WGL) \
	dispatch_wgl.c \
	$()
endif

# These are generated alongside the .c file.
$(GENERATED_GL_INCLUDES): $(GENERATED_GL_SOURCE)
$(GENERATED_GLX_INCLUDES): $(GENERATED_GLX_SOURCE)
$(GENERATED_EGL_INCLUDES): $(GENERATED_EGL_SOURCE)
$(GENERATED_WGL_INCLUDES): $(GENERATED_WGL_SOURCE)

$(GENERATED_GL_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/gl.xml
	$(MKDIR_P) $(top_builddir)/include/epoxy
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/gl.xml

$(GENERATED_GLX_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/glx.xml
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/glx.xml

$(GENERATED_EGL_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/egl.xml
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/egl.xml

$(GENERATED_WGL_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/wgl.xml
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/wgl.xml

EXTRA_DIST = \
	gen_dispatch.py \
	$()
