cmake_minimum_required (VERSION 3.0)
cmake_policy (VERSION 3.0)
if (NOT CMAKE_VERSION VERSION_LESS "3.1")
    cmake_policy (SET CMP0054 NEW)
endif ()
project (Epoxy VERSION 1.3.1 LANGUAGES C)
set (TARGET_NAME epoxy)
set (TARGET_VER 1.3.1_yaronct-4)
set (TARGET_ABI_VER 0)
set (TARGET_OUTPUT_NAME "${TARGET_NAME}")

set (CMAKE_C_VISIBILITY_PRESET "hidden")

if ((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
elseif (NOT CMAKE_VERSION VERSION_LESS "3.1")
    set (CMAKE_C_STANDARD_REQUIRED TRUE)
    set (CMAKE_C_STANDARD "99")
endif ()

if ((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -Wall -Wextra")
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    if (NOT CMAKE_C_FLAGS MATCHES "/W[0-4]")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif ()
endif ()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package (X11)
find_package (PythonInterp)

include (CMakeDependentOption)

option (EPOXY_REBUILD_FROM_SPECS "Whether to rebuild the auto-generated sources and headers from the specs" FALSE)
if (EPOXY_REBUILD_FROM_SPECS AND NOT PYTHONINTERP_FOUND)
    message (FATAL_ERROR "You've set \"EPOXY_REBUILD_FROM_SPECS\" to \"TRUE\", which requires a python interpreter, but one couldn't be found.")
endif ()

option (EPOXY_SUPPORT_EGL "Whether to build with EGL support" TRUE)
cmake_dependent_option (EPOXY_SUPPORT_WGL "Whether to build with WGL support" ON WIN32 OFF)
cmake_dependent_option (EPOXY_SUPPORT_GLX "Whether to build with GLX support" "${X11_FOUND}" "NOT (WIN32 OR ANDROID)" OFF)

option (EPOXY_BUILD_SHARED "Whether to build a shared library" TRUE)
option (EPOXY_BUILD_STATIC "Whether to build a static library" FALSE)

configure_file ("include/epoxy/config.h.in" "include/epoxy/config.h")

add_subdirectory (src)
