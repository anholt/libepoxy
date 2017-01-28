gl_generated = custom_target('gl_generated.h',
                             input: gl_registry,
                             output: [
                               'gl_generated.h',
                             ],
                             command: [
                               python,
                               gen_dispatch_py,
                               '--header',
                               '--no-source',
                               '--outputdir=@OUTDIR@',
                               '@INPUT@'
                             ],
                             install: true,
                             install_dir: join_paths(epoxy_includedir, 'epoxy'))

gen_headers = [ gl_generated ]
headers = [ 'gl.h', ]

if build_egl
  egl_generated = custom_target('egl_generated.h',
                                input: egl_registry,
                                output: [
                                  'egl_generated.h',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--header',
                                  '--no-source',
                                  '--outputdir=@OUTDIR@',
                                  '@INPUT@'
                                ],
                                install: true,
                                install_dir: join_paths(epoxy_includedir, 'epoxy'))
  gen_headers += [ egl_generated ]
  headers += [ 'egl.h' ]
endif

if build_glx
  glx_generated = custom_target('glx_generated.h',
                                input: glx_registry,
                                output: [
                                  'glx_generated.h',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--header',
                                  '--no-source',
                                  '--outputdir=@OUTDIR@',
                                  '@INPUT@'
                                ],
                                install: true,
                                install_dir: join_paths(epoxy_includedir, 'epoxy'))
  gen_headers += [ glx_generated ]
  headers += [ 'glx.h' ]
endif

if build_wgl
  wgl_generated = custom_target('wgl_generated.h',
                                input: wgl_registry,
                                output: [
                                  'wgl_generated.h',
                                ],
                                command: [
                                  python,
                                  gen_dispatch_py,
                                  '--header',
                                  '--no-source',
                                  '--outputdir=@OUTDIR@',
                                  '@INPUT@'
                                ],
                                install: true,
                                install_dir: join_paths(epoxy_includedir, 'epoxy'))
  gen_headers += [ wgl_generated ]
  headers += [ 'wgl.h' ]
endif

install_headers(headers, subdir: 'epoxy')
