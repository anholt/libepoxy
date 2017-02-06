has_gles1 = gles1_dep.found()
has_gles2 = gles2_dep.found()
build_x11_tests = build_glx and x11_dep.found()

test_cflags = common_cflags + [
  '-D_XOPEN_SOURCE',
  '-D_POSIX_C_SOURCE=200809L',
]

# Unconditionally built tests
test('header_guards',
     executable('header guards', 'headerguards.c',
                c_args: common_cflags,
                dependencies: libepoxy_dep,
                include_directories: libepoxy_inc))
test('misc_defines',
     executable('misc defines', 'miscdefines.c',
                c_args: common_cflags,
                dependencies: libepoxy_dep,
                include_directories: libepoxy_inc))
test('khronos_typedefs',
     executable('khronos typedefs', [
                  'khronos_typedefs.c',
                  'khronos_typedefs.h',
                  'khronos_typedefs_nonepoxy.c',
                ],
                c_args: common_cflags,
                dependencies: libepoxy_dep,
                include_directories: libepoxy_inc))

if build_egl and build_x11_tests
  egl_common_sources = [ 'egl_common.h', 'egl_common.c', ]
  egl_common_lib = static_library('egl_common',
                                  sources: egl_common_sources,
                                  dependencies: libepoxy_dep,
                                  include_directories: libepoxy_inc,
                                  c_args: common_cflags,
                                  install: false)

  egl_tests = [
    [ 'egl_has_extension_nocontext', [], [ 'egl_has_extension_nocontext.c' ], true, ],
    [ 'egl_gl', [], [ 'egl_gl.c' ], true, ],
    [ 'egl_gles1_without_glx', [ '-DGLES_VERSION=1', ], [ 'egl_without_glx.c' ], has_gles1, ],
    [ 'egl_gles2_without_glx', [ '-DGLES_VERSION=2', ], [ 'egl_without_glx.c' ], has_gles2, ],
  ]

  foreach test: egl_tests
    test_name = test[0]
    test_source = test[2]
    test_args = test[1]
    test_run = test[3]

    if test_run
      test_bin = executable(test_name, test_source,
                            c_args: test_cflags + test_args,
                            include_directories: libepoxy_inc,
                            dependencies: [ libepoxy_dep, x11_dep, egl_dep, dl_dep ],
                            link_with: egl_common_lib,
                            link_args: '-rdynamic')
      test(test_name, test_bin)
    endif
  endforeach
endif

if build_glx
  glx_common_sources = [ 'glx_common.h', 'glx_common.c', ]
  glx_common_lib = static_library('glx_common',
                                  sources: glx_common_sources,
                                  dependencies: libepoxy_dep,
                                  include_directories: libepoxy_inc,
                                  c_args: common_cflags,
                                  install: false)

  # glx_beginend links directly with the GL library, so we need to check it
  # separately
  test('glx_beginend', executable('glx_beginend', 'glx_beginend.c',
                                  c_args: test_cflags,
                                  include_directories: libepoxy_inc,
                                  dependencies: [ libepoxy_dep, x11_dep, gl_dep, dl_dep ],
                                  link_with: glx_common_lib))

  glx_tests = [
    [ 'glx_public_api', [ 'glx_public_api.c' ], [], [], true ],
    [ 'glx_public_api_core', [ 'glx_public_api_core.c' ], [], [], true ],
    [ 'glx_glxgetprocaddress_nocontext', [ 'glx_glxgetprocaddress_nocontext.c' ], [], [], true ],
    [ 'glx_has_extension_nocontext', [ 'glx_has_extension_nocontext.c' ], [], [], true ],
    [ 'glx_static', [ 'glx_static.c' ], [ '-DNEEDS_TO_BE_STATIC'], [ '-static' ], libtype == 'static' ],
    [ 'glx_shared_znow', [ 'glx_static.c', ], [], [ '-Wl,-z,now' ], has_znow ],
    [ 'glx_alias_prefer_same_name', [ 'glx_alias_prefer_same_name.c', 'dlwrap.c', 'dlwrap.h' ], [], [ '-rdynamic' ], not build_apple ],
    [ 'glx_gles2', [ 'glx_gles2.c', 'dlwrap.c', 'dlwrap.h' ], [], [ '-rdynamic' ], not build_apple ],
  ]

  foreach test: glx_tests
    test_name = test[0]
    test_source = test[1]
    test_c_args = test[2]
    test_link_args = test[3]
    test_run = test[4]

    if test_run
      test_bin = executable(test_name, test_source,
                            c_args: test_cflags + test_c_args,
                            include_directories: libepoxy_inc,
                            dependencies: [ libepoxy_dep, x11_dep, dl_dep ],
                            link_with: glx_common_lib,
                            link_args: test_link_args)
      test(test_name, test_bin)
    endif
  endforeach

  if not build_apple
    # GLX/EGL tests
    if build_egl
      glx_egl_sources = [
        'egl_and_glx_different_pointers.c',
        'dlwrap.c',
        'dlwrap.h',
      ]

      glx_egl_deps = [ libepoxy_dep, x11_dep, dl_dep ]
      glx_egl_link_flags = [ '-rdynamic' ]
      glx_egl_link_with = [ glx_common_lib, egl_common_lib ]

      glx_egl_tests = [
        [ 'egl_and_glx_different_pointers_glx', [ '-DUSE_GLX' ], false ],
        [ 'egl_and_glx_different_pointers_egl', [ '-DUSE_EGL' ], false ],
        [ 'egl_and_glx_different_pointers_egl_glx', [ '-DUSE_EGL', '-DUSE_GLX' ], true ],
      ]

      foreach test: glx_egl_tests
        test_name = test[0]
        test_c_args = test[1]
        test_should_fail = test[2]

        test_bin = executable(test_name, glx_egl_sources,
                              c_args: common_cflags + test_c_args,
                              include_directories: libepoxy_inc,
                              dependencies: glx_egl_deps,
                              link_with: glx_egl_link_with,
                              link_args: glx_egl_link_flags)
        test(test_name, test_bin, should_fail: test_should_fail)
      endforeach
    endif
  endif
endif

# WGL
if build_wgl
  wgl_common_sources = [ 'wgl_common.h', 'wgl_common.c', ]
  wgl_common_lib = static_library('wgl_common',
                                  sources: wgl_common_sources,
                                  dependencies: libepoxy_dep,
                                  include_directories: libepoxy_inc,
                                  c_args: common_cflags,
                                  install: false)

  wgl_tests = [
    [ 'wgl_core_and_exts', [ 'wgl_core_and_exts.c' ], [], ],
    [ 'wgl_per_context_funcptrs', [ 'wgl_per_context_funcptrs.c' ], [], ],
    [ 'wgl_usefontbitmaps', [ 'wgl_usefontbitmaps.c'], [], ],
    [ 'wgl_usefontbitmaps_unicode', [ 'wgl_usefontbitmaps.c' ], [ '-DUNICODE' ], ],
  ]

  foreach test: wgl_tests
    test_name = test[0]
    test_source = test[1]
    test_c_args = test[2]

    test_bin = executable(test_name, test_source,
                          c_args: test_cflags + test_c_args,
                          include_directories: libepoxy_inc,
                          dependencies: [ libepoxy_dep ],
                          link_with: wgl_common_lib)

    test(test_name, test_bin)
  endforeach
endif
