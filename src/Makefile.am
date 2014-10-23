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

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$()

AM_CFLAGS = \
	$(CWARNFLAGS) \
	$(VISIBILITY_CFLAGS) \
	$(X11_CFLAGS) \
	$(EGL_CFLAGS) \
	$()

epoxyincludedir = $(includedir)/epoxy
lib_LTLIBRARIES = libepoxy.la

nodist_epoxyinclude_DATA = \
	$(GENERATED_GL_INCLUDES) \
	$(INSTALL_GLX_INCLUDES) \
	$(INSTALL_EGL_INCLUDES) \
	$(INSTALL_WGL_INCLUDES) \
	$()

c_sources = $(EPOXY_C_SRC)
h_sources = $(EPOXY_SRC_HDR)

gen_c_sources = $(GENERATED_GL_SOURCE)
gen_headers = $(GENERATED_GL_INCLUDES)

if BUILD_EGL
INSTALL_EGL_INCLUDES = $(GENERATED_EGL_INCLUDES)
c_sources += $(EPOXY_EGL_C_SRC)
gen_c_sources += $(GENERATED_EGL_SOURCE)
gen_headers += $(GENERATED_EGL_INCLUDES)
endif

if BUILD_GLX
INSTALL_GLX_INCLUDES = $(GENERATED_GLX_INCLUDES)
c_sources += $(EPOXY_GLX_C_SRC)
gen_c_sources += $(GENERATED_GLX_SOURCE)
gen_headers += $(GENERATED_GLX_INCLUDES)
endif

if BUILD_WGL
INSTALL_WGL_INCLUDES = $(GENERATED_WGL_INCLUDES)
c_sources += $(EPOXY_WGL_C_SRC)
gen_c_sources += $(GENERATED_WGL_SOURCE)
gen_headers += $(GENERATED_WGL_INCLUDES)
endif

GENERATED_GL_INCLUDES = $(builddir)/../$(GL_GEN_HEADER)

GENERATED_GLX_INCLUDES = $(builddir)/../$(GLX_GEN_HEADER)

GENERATED_EGL_INCLUDES = $(builddir)/../$(EGL_GEN_HEADER)

GENERATED_WGL_INCLUDES = $(builddir)/../$(WGL_GEN_HEADER)

BUILT_SOURCES = \
	$(gen_c_sources) \
	$(gen_headers) \
	$()
CLEANFILES = $(BUILT_SOURCES)

libepoxy_la_SOURCES = \
	$(c_sources) \
	$(h_sources) \
	$()

nodist_libepoxy_la_SOURCES = \
	$(gen_c_sources) \
	$()

libepoxy_la_LDFLAGS = \
	-no-undefined \
        -Bsymbolic \
	$()

libepoxy_la_LIBADD = \
	$(EPOXY_LINK_LIBS) \
        $(DLOPEN_LIBS) \
	$()

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
