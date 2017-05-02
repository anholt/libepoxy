# Configuration file
configure_file(output: 'config.h', configuration: conf)

# List of generated sources:
#   - name of the generated file
#   - registry source file
#   - additional sources
generated_sources = [
  [ 'gl_generated_dispatch.c', gl_registry, [ 'dispatch_common.c', 'dispatch_common.h' ] ]
]

if build_egl
  generated_sources += [ [ 'egl_generated_dispatch.c', egl_registry, 'dispatch_egl.c' ] ]
endif

if build_glx
  generated_sources += [ [ 'glx_generated_dispatch.c', glx_registry, 'dispatch_glx.c' ] ]
endif

if build_wgl
  generated_sources += [ [ 'wgl_generated_dispatch.c', wgl_registry, 'dispatch_wgl.c' ] ]
endif

gen_sources = [ ]
sources = [ ]

foreach g: generated_sources
  gen_source = g[0]
  registry = g[1]
  source = g[2]

  generated = custom_target(gen_source,
                            input: registry,
                            output: [ gen_source ],
                            command: [
                              python,
                              gen_dispatch_py,
                              '--source',
                              '--no-header',
                              '--outputdir=@OUTDIR@',
                              '@INPUT@',
                            ])

  gen_sources += [ generated ]
  sources += [ source ]
endforeach

epoxy_sources = sources + gen_sources

common_ldflags = []

if host_system == 'linux'
  foreach f: [ '-Wl,-Bsymbolic', '-Wl,-z,relro', '-Wl,-z,now', ]
    if cc.has_argument(f)
      common_ldflags += f
    endif
  endforeach
endif

# Maintain compatibility with autotools; see: https://github.com/anholt/libepoxy/issues/108
if host_system == 'darwin'
  common_ldflags += [ '-compatibility_version=1', '-current_version=1.0', ]
endif

epoxy_deps = [ dl_dep, ]
if host_system == 'windows'
  epoxy_deps += [ opengl32_dep, gdi32_dep ]
endif

# Allow building a static version of epoxy
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
                                   c_args: common_cflags + visibility_cflags,
                                   link_args: common_ldflags)
  libepoxy = libepoxy_shared
endif

libepoxy_dep = declare_dependency(link_with: libepoxy,
                                  include_directories: libepoxy_inc,
                                  dependencies: epoxy_deps,
                                  sources: epoxy_headers)
