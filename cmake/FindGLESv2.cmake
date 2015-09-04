# Find GLESv2
#
# GLESv2_LIBRARY
# GLES_V2_FOUND

set (GLESv2_NAMES ${GLESv2_NAMES} GLESv2 libGLESv2)
find_library (GLESv2_LIBRARY NAMES ${GLESv2_NAMES} PATHS /opt/vc/lib)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (GLES_V2 DEFAULT_MSG GLESv2_LIBRARY)

mark_as_advanced (GLESv2_LIBRARY)
