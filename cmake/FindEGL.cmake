# Find EGL
#
# EGL_LIBRARY
# EGL_FOUND

set (EGL_NAMES ${EGL_NAMES} egl EGL libEGL)
find_library (EGL_LIBRARY NAMES ${EGL_NAMES} PATHS /opt/vc/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY)

mark_as_advanced(EGL_LIBRARY)
