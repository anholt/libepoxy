doc_sources = [
  join_paths(meson.source_root(), 'include/epoxy/gl.h'),
  join_paths(meson.source_root(), 'include/epoxy/egl.h'),
  join_paths(meson.source_root(), 'include/epoxy/glx.h'),
  join_paths(meson.source_root(), 'include/epoxy/wgl.h'),

  join_paths(meson.source_root(), 'src/dispatch_common.c'),
  join_paths(meson.source_root(), 'src/dispatch_egl.c'),
  join_paths(meson.source_root(), 'src/dispatch_glx.c'),
  join_paths(meson.source_root(), 'src/dispatch_wgl.c'),
]

doxyfile_conf = configuration_data()
doxyfile_conf.set('PACKAGE_NAME', meson.project_name())
doxyfile_conf.set('PACKAGE_VERSION', meson.project_version())
doxyfile_conf.set('top_srcdir', meson.source_root())
doxyfile_conf.set('top_builddir', meson.build_root())
doxyfile_conf.set('DOC_SOURCES', ' '.join(doc_sources))

doxyfile = configure_file(input: 'Doxyfile.in',
                          output: 'Doxyfile',
                          configuration: doxyfile_conf,
                          install: false)

docdir = join_paths(epoxy_datadir, 'doc')

html_target = custom_target('epoxy-docs',
                            input: [ doxyfile ] + doc_sources,
                            output: [ 'html' ],
                            command: [ doxygen, doxyfile ],
                            install: true,
                            install_dir: join_paths(docdir, 'epoxy'))
