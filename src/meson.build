common_sources = [
  'dispatch_common.c',
  'dispatch_common.h',
]

# Configuration file
configure_file(output: 'config.h', configuration: conf)

gl_generated = custom_target('gl_generated_dispatch.c',
                             input: gl_registry,
                             output: [
                               'gl_generated_dispatch.c',
                             ],
                             command: [
                               python,
                               gen_dispatch_py,
                               '--source',
                               '--no-header',
                               '--outputdir=' + meson.current_build_dir(),
                               '@INPUT@',
                             ])

gen_sources = [ gl_generated ]
sources = common_sources 

if build_egl
  egl_generated = custom_target('egl_generated_dispatch.c',
                                input: egl_registry,
                                output: [
                                  'egl_generated_dispatch.c',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--source',
                                  '--no-header',
                                  '--outputdir=' + meson.current_build_dir(),
                                  '@INPUT@',
                                ])
  gen_sources += [ egl_generated ]                              
  sources += [ 'dispatch_egl.c' ]
endif

if build_glx
  glx_generated = custom_target('glx_generated_dispatch.c',
                                input: glx_registry,
                                output: [
                                  'glx_generated_dispatch.c',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--source',
                                  '--no-header',
                                  '--outputdir=' + meson.current_build_dir(),
                                  '@INPUT@',
                                ])
  gen_sources += [ glx_generated ]
  sources += [ 'dispatch_glx.c' ]
endif

if build_wgl
  wgl_generated = custom_target('wgl_generated_dispatch.c',
                                input: wgl_registry,
                                output: [
                                  'wgl_generated_dispatch.c',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--source',
                                  '--no-header',
                                  '--outputdir=' + meson.current_build_dir(),
                                  '@INPUT@',
                                ])
  gen_sources += [ wgl_generated ]
  sources += [ 'dispatch_wgl.c' ]
endif

epoxy_sources = common_sources + sources + gen_sources
epoxy_headers = gen_headers
foreach h: headers
  epoxy_headers += join_paths(meson.source_root(), 'include/epoxy/@0@'.format(h))
endforeach

if cc.get_id() == 'gcc'
  common_ldflags = [ '-Wl,-Bsymbolic', ]
else
  common_ldflags = []
endif

epoxy_deps = [ dl_dep, ]
if host_system == 'windows'
  epoxy_deps += [ opengl32_dep, gdi32_dep ]
endif

# Allow building a static version of epoxy
libtype = get_option('default_library')

if libtype != 'shared'
  libepoxy_static = static_library('epoxy',
                                   sources: epoxy_sources + epoxy_headers,
                                   install: true,
                                   dependencies: epoxy_deps,
                                   include_directories: libepoxy_inc,
                                   c_args: common_cflags + visibility_cflags,
                                   link_args: common_ldflags)
  libepoxy = libepoxy_static
endif

if libtype != 'static'
  libepoxy_shared = shared_library('epoxy',
                                   sources: epoxy_sources + epoxy_headers,
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
                                  dependencies: epoxy_deps,
                                  sources: epoxy_headers)
