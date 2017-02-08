project('libepoxy', 'c', version: '1.4.0',
        default_options: [
          'buildtype=debugoptimized',
          'c_std=gnu99',
          'warning_level=1',
        ],
        license: 'MIT',
        meson_version: '>= 0.36.0')

epoxy_version = meson.project_version().split('.')
epoxy_major_version = epoxy_version[0].to_int()
epoxy_minor_version = epoxy_version[1].to_int()
epoxy_micro_version = epoxy_version[2].to_int()

epoxy_prefix = get_option('prefix')
epoxy_libdir = join_paths(epoxy_prefix, get_option('libdir'))
epoxy_datadir = join_paths(epoxy_prefix, get_option('datadir'))
epoxy_includedir = join_paths(epoxy_prefix, get_option('includedir'))

cc = meson.get_compiler('c')
host_system = host_machine.system()

conf = configuration_data()
conf.set_quoted('PACKAGE_NAME', meson.project_name())
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('PACKAGE_STRING', '@0@-@1@'.format(meson.project_name(), meson.project_version()))
conf.set_quoted('PACKAGE_DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
conf.set_quoted('PACKAGE_LIBDIR', join_paths(get_option('prefix'), get_option('libdir')))
conf.set_quoted('PACKAGE_LOCALEDIR', join_paths(get_option('prefix'), get_option('datadir'), 'locale'))
conf.set_quoted('PACKAGE_LIBEXECDIR', join_paths(get_option('prefix'), get_option('libexecdir')))
conf.set('HAVE_KHRPLATFORM_H', cc.has_header('KHR/khrplatform.h', required: false))

# GLX can be used on different platforms, so we expose a
# configure time switch to enable or disable it; in case
# the "auto" default value is set, we only enable GLX
# support on Linux and Unix
enable_glx = get_option('enable-glx')
if enable_glx == 'auto'
  if host_system == 'windows'
    build_glx = false
  elif host_system == 'darwin'
    build_glx = false
  elif host_system == 'android'
    build_glx = false
  else
    build_glx = true
  endif
elif enable_glx == 'yes'
  build_glx = true
elif enable_glx == 'no'
  build_glx = false
endif

# The remaining platform specific API for GL/GLES are enabled
# depending on the platform we're building for
if host_system == 'windows'
  build_egl = false
  build_apple = false
  build_wgl = true
  has_znow = true
elif host_system == 'darwin'
  build_egl = false
  build_apple = true
  build_wgl = false
  has_znow = false
else
  build_egl = true
  build_apple = false
  build_wgl = false
  has_znow = true
endif

conf.set10('ENABLE_GLX', build_glx)

# Compiler flags, taken from the Xorg macros
test_cflags = [
  '-Wpointer-arith',
  '-Wmissing-declarations',
  '-Wformat=2',
  '-Wstrict-prototypes',
  '-Wmissing-prototypes',
  '-Wnested-externs',
  '-Wbad-function-cast',
  '-Wold-style-definition',
  '-Wdeclaration-after-statement',
  '-Wunused',
  '-Wuninitialized',
  '-Wshadow',
  '-Wmissing-noreturn',
  '-Wmissing-format-attribute',
  '-Wredundant-decls',
  '-Wlogical-op',
  '-Werror=implicit',
  '-Werror=nonnull',
  '-Werror=init-self',
  '-Werror=main',
  '-Werror=missing-braces',
  '-Werror=sequence-point',
  '-Werror=return-type',
  '-Werror=trigraphs',
  '-Werror=array-bounds',
  '-Werror=write-strings',
  '-Werror=address',
  '-Werror=int-to-pointer-cast',
  '-Werror=pointer-to-int-cast',
  '-fno-strict-aliasing',
  '-Wno-int-conversion',
]
common_cflags = []
foreach cflag: test_cflags
  if cc.has_argument(cflag)
    common_cflags += [ cflag ]
  endif
endforeach

# Visibility compiler flags
visibility_cflags = []
if get_option('default_library') != 'static'
  if host_system == 'windows'
    conf.set('DLL_EXPORT', true)
    if cc.get_id() == 'msvc'
      conf.set('EPOXY_PUBLIC', '__declspec(dllexport) extern')
    else
      conf.set('EPOXY_PUBLIC', '__attribute__((visibility("default"))) __declspec(dllexport) extern')
      visibility_cflags += [ '-fvisibility=hidden' ]
    endif
  else
    conf.set('EPOXY_PUBLIC', '__attribute__((visibility("default")))')
    visibility_cflags += [ '-fvisibility=hidden' ]
  endif
endif

# Dependencies
dl_dep = cc.find_library('dl', required: false)
gl_dep = dependency('gl', required: false)
egl_dep = dependency('egl', required: false)

# Optional dependencies for tests
x11_dep = dependency('x11', required: false)
gles1_dep = cc.find_library('libGLESv1_CM', required: false)
gles2_dep = cc.find_library('libGLESv2', required: false)

# On windows, the DLL has to have all of its functions
# resolved at link time, so we have to link directly aginst
# opengl32.  But that's the only GL provider, anyway.
if host_system == 'windows'
  opengl32_dep = cc.find_library('opengl32', required: true)

  # When building against static libraries, we need to control
  # the order of the dependencies, and gdi32 provides symbols
  # needed when using opengl32, like SetPixelFormat and
  # ChoosePixelFormat. This is mostly a workaround for older
  # versions of Meson.
  gdi32_dep = cc.find_library('gdi32', required: true)
endif

# PkgConfig file
pkgconf = configuration_data()
pkgconf.set('prefix', epoxy_prefix)
pkgconf.set('exec_prefix', epoxy_prefix)
pkgconf.set('libdir', epoxy_libdir)
pkgconf.set('includedir', epoxy_includedir)
pkgconf.set10('epoxy_has_glx', build_glx)
pkgconf.set10('epoxy_has_egl', build_egl)
pkgconf.set10('epoxy_has_wgl', build_wgl)
pkgconf.set('PACKAGE_VERSION', meson.project_version())
if dl_dep.found()
  pkgconf.set('DLOPEN_LIBS', '-ldl')
endif

configure_file(input: 'epoxy.pc.in',
               output: 'epoxy.pc',
               configuration: pkgconf,
               install: true,
               install_dir: join_paths(epoxy_libdir, 'pkgconfig'))

# Generates the dispatch tables
gen_dispatch_py = find_program('src/gen_dispatch.py')

gl_registry = files('registry/gl.xml')
egl_registry = files('registry/egl.xml')
glx_registry = files('registry/glx.xml')
wgl_registry = files('registry/wgl.xml')

libepoxy_inc = [
  include_directories('include'),
  include_directories('src'),
]

subdir('include/epoxy')
subdir('src')
subdir('test')

if get_option('enable-docs')
  doxygen = find_program('doxygen', required: false)
  if doxygen.found()
    subdir('doc')
  else
    message('Documentation disabled without doxygen')
  endif
endif
