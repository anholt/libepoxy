cmake_minimum_required(VERSION 3.18)
project(libepoxy
    VERSION 1.5.11
    LANGUAGES C
)

# Set C standard and default build type
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DebugOptimized)
endif()

# Define installation paths
include(GNUInstallDirs)
set(EPOXY_PREFIX ${CMAKE_INSTALL_PREFIX})
set(EPOXY_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR})
set(EPOXY_DATADIR ${CMAKE_INSTALL_FULL_DATADIR})
set(EPOXY_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})

# Configuration header
include(CheckIncludeFile)
check_include_file("KHR/khrplatform.h" HAVE_KHRPLATFORM_H)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# Options mirroring meson_options.txt
option(ENABLE_GLX "Enable GLX support (auto/yes/no)" "AUTO")
option(ENABLE_EGL "Enable EGL support (auto/yes/no)" "AUTO")
option(ENABLE_X11 "Enable X11 support (GLX or EGL-X11)" ON)
option(BUILD_TESTS "Build the test suite" ON)
option(BUILD_DOCS "Enable generating the Epoxy API reference (depends on Doxygen)" OFF)

# Platform detection and logic
if(WIN32)
    set(BUILD_WGL ON)
    set(HAS_ZNOW OFF)
elseif(APPLE)
    set(BUILD_WGL OFF)
    set(HAS_ZNOW OFF)
else()
    set(BUILD_WGL OFF)
    set(HAS_ZNOW ON)
endif()

# GLX logic
if(ENABLE_GLX STREQUAL "AUTO")
    if(CMAKE_SYSTEM_NAME MATCHES "Windows|Darwin|Android|Haiku")
        set(BUILD_GLX OFF)
    else()
        set(BUILD_GLX ON)
    endif()
elseif(ENABLE_GLX)
    set(BUILD_GLX ON)
else()
    set(BUILD_GLX OFF)
endif()

# EGL logic
if(ENABLE_EGL STREQUAL "AUTO")
    if(WIN32 OR APPLE)
        set(BUILD_EGL OFF)
    else()
        set(BUILD_EGL ON)
    endif()
elseif(ENABLE_EGL)
    set(BUILD_EGL ON)
else()
    set(BUILD_EGL OFF)
endif()

# X11 validation
if(NOT ENABLE_X11 AND BUILD_GLX)
    message(FATAL_ERROR "GLX support is enabled, but X11 was disabled")
endif()
if(NOT ENABLE_X11)
    set(BUILD_GLX OFF)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(
        /we4002 /we4003 /w14010 /we4013 /w14016 /we4020 /we4021
        /we4027 /we4029 /we4033 /we4035 /we4045 /we4047 /we4049
        /we4053 /we4071 /we4819 /utf-8
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(
        -Wpointer-arith -Wmissing-declarations -Wformat=2
        -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs
        -Wbad-function-cast -Wold-style-definition -Wdeclaration-after-statement
        -Wunused -Wuninitialized -Wshadow -Wmissing-noreturn
        -Wmissing-format-attribute -Wredundant-decls
        # -Wlogical-op not supported by AppleClang, skipping for now
        -Werror=implicit -Werror=nonnull -Werror=init-self -Werror=main
        -Werror=missing-braces -Werror=sequence-point -Werror=return-type
        -Werror=trigraphs -Werror=array-bounds -Werror=write-strings
        -Werror=address -Werror=int-to-pointer-cast -Werror=pointer-to-int-cast
        -fno-strict-aliasing -Wno-int-conversion
    )
endif()

# Visibility flags for shared libraries
if(BUILD_SHARED_LIBS)
    if(WIN32)
        add_definitions(-DDLL_EXPORT -DEPOXY_PUBLIC=__declspec(dllexport) extern)
        if(NOT MSVC)
            add_compile_options(-fvisibility=hidden)
        endif()
    else()
        add_definitions(-DEPOXY_PUBLIC=__attribute__((visibility("default"))) extern)
        add_compile_options(-fvisibility=hidden)
    endif()
endif()

# Dependencies
find_library(DL_LIBRARY dl)
find_package(OpenGL REQUIRED)
if(BUILD_EGL)
    find_package(EGL)
endif()
if(ENABLE_X11)
    find_package(X11)
endif()
if(WIN32)
    find_library(GDI32_LIBRARY gdi32 REQUIRED)
endif()

# Python for dispatch generation
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(GEN_DISPATCH_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/gen_dispatch.py)

# Generated sources and headers
set(GENERATED_SOURCES)
set(GENERATED_HEADERS)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/epoxy)

# GL (always generated)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/epoxy/gl_generated_dispatch.c
           ${CMAKE_CURRENT_BINARY_DIR}/epoxy/gl_generated.h
    COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --source --no-header --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/gl.xml
    COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --header --no-source --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/gl.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/registry/gl.xml ${GEN_DISPATCH_SCRIPT}
    COMMENT "Generating GL dispatch files"
)
list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/epoxy/gl_generated_dispatch.c)
list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/epoxy/gl_generated.h)

# EGL (conditional)
if(BUILD_EGL)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/epoxy/egl_generated_dispatch.c
               ${CMAKE_CURRENT_BINARY_DIR}/epoxy/egl_generated.h
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --source --no-header --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/egl.xml
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --header --no-source --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/egl.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/registry/egl.xml ${GEN_DISPATCH_SCRIPT}
        COMMENT "Generating EGL dispatch files"
    )
    list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/epoxy/egl_generated_dispatch.c)
    list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/epoxy/egl_generated.h)
endif()

# GLX (conditional)
if(BUILD_GLX)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/epoxy/glx_generated_dispatch.c
               ${CMAKE_CURRENT_BINARY_DIR}/epoxy/glx_generated.h
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --source --no-header --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/glx.xml
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --header --no-source --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/glx.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/registry/glx.xml ${GEN_DISPATCH_SCRIPT}
        COMMENT "Generating GLX dispatch files"
    )
    list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/epoxy/glx_generated_dispatch.c)
    list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/epoxy/glx_generated.h)
endif()

# WGL (conditional)
if(BUILD_WGL)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/epoxy/wgl_generated_dispatch.c
               ${CMAKE_CURRENT_BINARY_DIR}/epoxy/wgl_generated.h
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --source --no-header --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/wgl.xml
        COMMAND ${Python3_EXECUTABLE} ${GEN_DISPATCH_SCRIPT} --header --no-source --outputdir=${CMAKE_CURRENT_BINARY_DIR}/epoxy ${CMAKE_CURRENT_SOURCE_DIR}/registry/wgl.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/registry/wgl.xml ${GEN_DISPATCH_SCRIPT}
        COMMENT "Generating WGL dispatch files"
    )
    list(APPEND GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/epoxy/wgl_generated_dispatch.c)
    list(APPEND GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/epoxy/wgl_generated.h)
endif()

# Source files
set(EPOXY_SOURCES
    src/dispatch_common.c
    ${GENERATED_SOURCES}
)

# Headers
set(EPOXY_HEADERS
    include/epoxy/common.h
    include/epoxy/gl.h
    ${GENERATED_HEADERS}
)

# Library definition
add_library(epoxy ${EPOXY_SOURCES} ${EPOXY_HEADERS})
target_include_directories(epoxy
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${EPOXY_INCLUDEDIR}>
)
target_link_libraries(epoxy
    PUBLIC
        OpenGL::GL
        $<$<BOOL:${DL_LIBRARY}>:${DL_LIBRARY}>
        $<$<BOOL:${EGL_FOUND}>:EGL::EGL>
        $<$<BOOL:${X11_FOUND}>:X11::X11>
        $<$<BOOL:${WIN32}>:${GDI32_LIBRARY}>
)

# Ensure generated files are built before the library
add_custom_target(generate_dispatch_files
    DEPENDS ${GENERATED_SOURCES} ${GENERATED_HEADERS}
)
add_dependencies(epoxy generate_dispatch_files)

# Installation
install(TARGETS epoxy
    EXPORT epoxyTargets
    LIBRARY DESTINATION ${EPOXY_LIBDIR}
    ARCHIVE DESTINATION ${EPOXY_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES ${EPOXY_HEADERS} DESTINATION ${EPOXY_INCLUDEDIR}/epoxy)

# Export targets for downstream use
install(EXPORT epoxyTargets
    FILE epoxyTargets.cmake
    NAMESPACE epoxy::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/epoxy
)