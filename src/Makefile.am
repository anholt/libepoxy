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

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$()

AM_CFLAGS = $(CWARNFLAGS) $(VISIBILITY_CFLAGS)

epoxyincludedir = $(includedir)/epoxy
lib_LTLIBRARIES = libepoxy.la

epoxyinclude_DATA = \
	$(GL_INCLUDES) \
	$(GLX_INCLUDES) \
	$(EGL_INCLUDES) \
	$()

GL_INCLUDES = \
	../include/epoxy/gl.h \
	$(GENERATED_GL_INCLUDES) \
	$()

GLX_INCLUDES = \
	../include/epoxy/glx.h \
	$(GENERATED_GLX_INCLUDES) \
	$()

EGL_INCLUDES = \
	../include/epoxy/egl.h \
	$(GENERATED_EGL_INCLUDES) \
	$()

GENERATED_GL_INCLUDES = \
	$(builddir)/../include/epoxy/gl_generated.h \
	$(builddir)/../include/epoxy/gl_generated_vtable_defines.h \
	$()

GENERATED_GLX_INCLUDES = \
	$(tuilddir)/../include/epoxy/glx_generated.h \
	$(builddir)/../include/epoxy/glx_generated_vtable_defines.h \
	$()

GENERATED_EGL_INCLUDES = \
	$(tuilddir)/../include/epoxy/egl_generated.h \
	$(builddir)/../include/epoxy/egl_generated_vtable_defines.h \
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

BUILT_SOURCES = \
	$(GENERATED_GL) \
	$(GENERATED_GLX) \
	$(GENERATED_EGL) \
	$()
CLEANFILES = $(BUILT_SOURCES)

libepoxy_la_SOURCES = \
	dispatch_common.c \
	dispatch_common.h \
	$(BUILT_SOURCES)
	$()

# These are generated alongside the .c file.
$(GENERATED_GL_INCLUDES): $(GENERATED_GL_SOURCE)
$(GENERATED_GLX_INCLUDES): $(GENERATED_GLX_SOURCE)
$(GENERATED_EGL_INCLUDES): $(GENERATED_EGL_SOURCE)

$(GENERATED_GL_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/gl.xml
	$(MKDIR_P) $(top_builddir)/include/epoxy
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/gl.xml

$(GENERATED_GLX_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/glx.xml
	$(MKDIR_P) $(top_builddir)/include/epoxy
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/glx.xml

$(GENERATED_EGL_SOURCE): $(srcdir)/gen_dispatch.py $(top_srcdir)/registry/egl.xml
	$(MKDIR_P) $(top_builddir)/include/epoxy
	$(AM_V_GEN)$(PYTHON) $(srcdir)/gen_dispatch.py --dir $(top_builddir) $(top_srcdir)/registry/egl.xml
