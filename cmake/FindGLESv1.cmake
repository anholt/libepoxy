# Find GLESv1
#
# GLESv1_LIBRARY
# GLES_V1_FOUND

set(GLESv1_NAMES ${GLESv1_NAMES} GLES_CM GLESv1_CM libGLES_CM)
find_library (GLESv1_LIBRARY NAMES ${GLESv1_NAMES} PATHS /opt/vc/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args (GLES_V1 DEFAULT_MSG GLESv1_LIBRARY)

mark_as_advanced (GLESv1_LIBRARY)
