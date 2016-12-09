common_sources = [
  'dispatch_common.c',
  'dispatch_common.h',
]

# Configuration file
configure_file(output: 'config.h', configuration: conf)

# Generate the dispatch tables
gen_dispatch_py = find_program('gen_dispatch.py')

gen_dispatch = generator(gen_dispatch_py,
			 output: [ '@BASENAME@_generated.h', '@BASENAME@_generated_dispatch.c', ],
                         arguments: [ '--dir=@BUILD_DIR@', '@INPUT@' ])

gl_generated = gen_dispatch.process(join_paths(meson.source_root(), 'registry/gl.xml'))
gl_headers = [ join_paths(meson.source_root(), 'include/epoxy/gl.h'), 'gl_generated.h', ]
gl_sources = gl_generated

egl_headers = []
egl_sources = []
if build_egl
  egl_generated = gen_dispatch.process(join_paths(meson.source_root(), 'registry/egl.xml'))
  egl_headers += [ join_paths(meson.source_root(), 'include/epoxy/egl.h'), 'egl_generated.h', ]
  egl_sources += [ egl_generated, 'dispatch_egl.c', ]
endif

glx_headers = []
glx_sources = []
if build_glx
  glx_generated = gen_dispatch.process(join_paths(meson.source_root(), 'registry/glx.xml'))
  glx_headers += [ join_paths(meson.source_root(), 'include/epoxy/glx.h'), 'glx_generated.h', ]
  glx_sources += [ glx_generated, 'dispatch_glx.c', ]
endif

wgl_headers = []
wgl_sources = []
if build_wgl
  wgl_generated = gen_dispatch.process(join_paths(meson.source_root(), 'registry/wgl.xml'))
  wgl_headers += [ join_paths(meson.source_root(), 'include/epoxy/wgl.h'), 'wgl_generated.h', ]
  wgl_sources += [ wgl_generated, 'dispatch_wgl.c', ]
endif

if cc.get_id() == 'msvc'
  common_ldflags = []
else
  common_ldflags = [ '-Wl,-Bsymbolic', ]
endif

epoxy_deps = [ dl_dep, ]
if host_system == 'windows'
  ogl_dep = cc.find_library('opengl32', required: true)
  epoxy_deps += [ ogl_dep, ]
endif

libepoxy_inc = [ epoxy_include, include_directories('.'), ]

# Allow building a static version of epoxy
libtype = get_option('default_library')

if libtype != 'shared'
  libepoxy_static = static_library('epoxy',
                                   common_sources + [ gl_sources, egl_sources, glx_sources, wgl_sources ],
                                   install: true,
                                   dependencies: epoxy_deps,
                                   include_directories: libepoxy_inc,
                                   c_args: common_cflags,
                                   link_args: common_ldflags)
  libepoxy = libepoxy_static
endif

if libtype != 'static'
  libepoxy_shared = shared_library('epoxy',
                                   sources: common_sources + [ gl_sources, egl_sources, glx_sources, wgl_sources ],
                                   version: '0.0.0',
                                   install: true,
                                   dependencies: epoxy_deps,
                                   include_directories: libepoxy_inc,
                                   c_args: common_cflags,
                                   link_args: common_ldflags)
  libepoxy = libepoxy_shared
endif

libepoxy_dep = declare_dependency(link_with: libepoxy,
                                  include_directories: libepoxy_inc,
                                  dependencies: dl_dep)

install_headers(gl_headers + glx_headers + egl_headers + wgl_headers, subdir: 'epoxy')
