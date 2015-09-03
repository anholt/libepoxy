if (EPOXY_BUILD_TESTS)

    if (EPOXY_BUILD_SHARED)
        set (TESTS_TARGET_NAME "${EPOXY_TARGET_NAME}_shared")
    else ()
        set (TESTS_TARGET_NAME "${EPOXY_TARGET_NAME}_static")
    endif ()

    if (EPOXY_SUPPORT_EGL)
        if (EGL_FOUND)
            set (EPOXY_BUILD_EGL_TESTS TRUE)
            set (EPOXY_EGL_TESTS_COMMON egl_common.c egl_common.h)
        else ()
            set (EPOXY_BUILD_EGL_TESTS FALSE)
            message (WARNING "EGL not found - EGL tests will not be built!")
        endif ()
    endif ()
    if (EPOXY_SUPPORT_GLX)
        set (EPOXY_GLX_TESTS_COMMON glx_common.c glx_common.h)
    endif ()
    if (EPOXY_SUPPORT_WGL)
        set (EPOXY_WGL_TESTS_COMMON wgl_common.c wgl_common.h)
    endif ()

    set (EPOXY_LINKAGE_TYPE_SHARED SHARED)
    set (EPOXY_LINKAGE_TYPE_STATIC STATIC)
    
    function (epoxy_add_test EPOXY_TEST_NAME EPOXY_TEST_CODE)
      # Additional optional arguments: "EPOXY_TEST_DEPS EPOXY_TEST_COMPILE_DEFS EPOXY_TEST_LINK_FLAGS"
      #                                "EPOXY_LINKAGE_TYPE"
      
        if (ARGC GREATER 2)
            set (EPOXY_TEST_DEPS "${ARGV2}")
        else ()
            set (EPOXY_TEST_DEPS "")
        endif ()
        if (ARGC GREATER 3)
            set (EPOXY_TEST_COMPILE_DEFS "${ARGV3}")
        else ()
            set (EPOXY_TEST_COMPILE_DEFS "")
        endif ()
        if (ARGC GREATER 4)
            set (EPOXY_TEST_LINK_FLAGS "${ARGV4}")
        else ()
            set (EPOXY_TEST_LINK_FLAGS "")
        endif ()
        if (ARGC GREATER 5)
            set (EPOXY_TEST_LINKAGE_TYPE "${ARGV5}")
        else ()
            set (EPOXY_TEST_LINKAGE_TYPE BOTH)
        endif ()
        
        if (EPOXY_BUILD_SHARED AND NOT EPOXY_TEST_LINKAGE_TYPE STREQUAL EPOXY_LINKAGE_TYPE_STATIC)
            set (EPOXY_TEST_TARGET_NAME "test_shared_${EPOXY_TEST_NAME}")
            add_executable (${EPOXY_TEST_TARGET_NAME} ${EPOXY_TEST_CODE})
            target_link_libraries (${EPOXY_TEST_TARGET_NAME} "${EPOXY_TARGET_NAME}_shared" ${EPOXY_TEST_DEPS})
            target_compile_definitions (${EPOXY_TEST_TARGET_NAME} PRIVATE
                                        ${EPOXY_TEST_COMPILE_DEFS})
            target_include_directories (${EPOXY_TEST_TARGET_NAME} PRIVATE ${EPOXY_INCLUDE_DIRS})
            set_target_properties (${EPOXY_TEST_TARGET_NAME} PROPERTIES
                                   LINK_FLAGS "${EPOXY_TEST_LINK_FLAGS}")
            add_test (NAME "${EPOXY_TEST_TARGET_NAME}"
                      COMMAND "${CMAKE_COMMAND}"
                          "-DEPOXY_TEST_CMD=$<TARGET_FILE:${EPOXY_TEST_TARGET_NAME}>"
                          "-DEPOXY_SHARED_LIB=$<TARGET_FILE:${EPOXY_TARGET_NAME}_shared>"
                          "-P" "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/epoxy_run_test_wrapper.cmake")
        endif ()
        
        if (EPOXY_BUILD_STATIC AND NOT EPOXY_TEST_LINKAGE_TYPE STREQUAL EPOXY_LINKAGE_TYPE_SHARED)
            set (EPOXY_TEST_TARGET_NAME "test_static_${EPOXY_TEST_NAME}")
            add_executable (${EPOXY_TEST_TARGET_NAME} ${EPOXY_TEST_CODE})
            target_link_libraries (${EPOXY_TEST_TARGET_NAME} "${EPOXY_TARGET_NAME}_static" ${EPOXY_TEST_DEPS})
            target_compile_definitions (${EPOXY_TEST_TARGET_NAME} PRIVATE
                                        ${EPOXY_TEST_COMPILE_DEFS} EPOXY_STATIC_LIB)
            target_include_directories (${EPOXY_TEST_TARGET_NAME} PRIVATE ${EPOXY_INCLUDE_DIRS})
            set_target_properties (${EPOXY_TEST_TARGET_NAME} PROPERTIES
                                   LINK_FLAGS "${EPOXY_TEST_LINK_FLAGS}")
            add_test (NAME "${EPOXY_TEST_TARGET_NAME}"
                      COMMAND "${CMAKE_COMMAND}"
                          "-DEPOXY_TEST_CMD=$<TARGET_FILE:${EPOXY_TEST_TARGET_NAME}>"
                          "-P" "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/epoxy_run_test_wrapper.cmake")
        endif ()
        
    endfunction ()

    if (EPOXY_BUILD_EGL_TESTS AND X11_FOUND)

        epoxy_add_test (egl_has_extension_nocontext "${EPOXY_EGL_TESTS_COMMON};egl_has_extension_nocontext.c"
                        "${X11_LIBRARIES}")
        epoxy_add_test (egl_gl "${EPOXY_EGL_TESTS_COMMON};egl_gl.c" "${X11_LIBRARIES}")
        if (GLESv1_FOUND)
            epoxy_add_test (egl_gles1_without_glx "${EPOXY_EGL_TESTS_COMMON};egl_without_glx.c" "${X11_LIBRARIES}"
                            "GLES_VERSION=1")
        endif ()
        if (GLESv2_FOUND)
            epoxy_add_test (egl_gles2_without_glx "${EPOXY_EGL_TESTS_COMMON};egl_without_glx.c" "${X11_LIBRARIES}"
                            "GLES_VERSION=2")
        endif ()
    endif()

    if (EPOXY_BUILD_EGL_TESTS AND EPOXY_SUPPORT_GLX AND NOT APPLE)
    
        #epoxy_add_test (egl_and_glx_different_pointers "${EPOXY_EGL_TESTS_COMMON};${EPOXY_GLX_TESTS_COMMON};egl_and_glx_different_pointers.c;dlwrap.c;dlwrap.h" "${X11_LIBRARIES}" "USE_EGL;USE_GLX" "-rdynamic")
        epoxy_add_test (egl_different_pointers
          "${EPOXY_EGL_TESTS_COMMON};${EPOXY_GLX_TESTS_COMMON};egl_and_glx_different_pointers.c;dlwrap.c;dlwrap.h"
          "${X11_LIBRARIES}" "USE_EGL" "-rdynamic")
        epoxy_add_test (glx_different_pointers
          "${EPOXY_EGL_TESTS_COMMON};${EPOXY_GLX_TESTS_COMMON};egl_and_glx_different_pointers.c;dlwrap.c;dlwrap.h"
          "${X11_LIBRARIES}" "USE_GLX" "-rdynamic")
    endif ()

    if (EPOXY_SUPPORT_GLX)
    
        if (NOT APPLE)
            epoxy_add_test (glx_alias_prefer_same_name
              "${EPOXY_GLX_TESTS_COMMON};glx_alias_prefer_same_name.c;dlwrap.c;dlwrap.h" "${X11_LIBRARIES}"
              "" "-rdynamic")
            if (GLESv2_FOUND)
                epoxy_add_test (test_glx_gles2
                  "${EPOXY_GLX_TESTS_COMMON};glx_gles2.c;dlwrap.c;dlwrap.h" "${X11_LIBRARIES}" "" "-rdynamic")
            endif ()
        endif ()
        
        epoxy_add_test (test_glx_beginend "${EPOXY_GLX_TESTS_COMMON};glx_beginend.c"
                        "${OPENGL_gl_LIBRARY};${X11_LIBRARIES}")
        epoxy_add_test (glx_public_api "${EPOXY_GLX_TESTS_COMMON};glx_public_api.c" "${X11_LIBRARIES}")
        epoxy_add_test (glx_public_api_core "${EPOXY_GLX_TESTS_COMMON};glx_public_api_core.c" "${X11_LIBRARIES}")
        epoxy_add_test (glx_getprocaddress_nocontext
          "${EPOXY_GLX_TESTS_COMMON};glx_glxgetprocaddress_nocontext.c" "${X11_LIBRARIES}")
        epoxy_add_test (glx_has_extension_nocontext "${EPOXY_GLX_TESTS_COMMON};glx_has_extension_nocontext.c"
                        "${X11_LIBRARIES}")

        epoxy_add_test (glx_shared_znow "${EPOXY_GLX_TESTS_COMMON};glx_static.c"
                        "${X11_LIBRARIES}" "" "-Wl,-z,now" SHARED)
        epoxy_add_test (glx_static "${EPOXY_GLX_TESTS_COMMON};glx_static.c"
                        "${X11_LIBRARIES}" "NEEDS_TO_BE_STATIC" "" STATIC)
    endif ()

	epoxy_add_test (headerguards headerguards.c)
	epoxy_add_test (miscdefines miscdefines.c)

    if (EPOXY_SUPPORT_WGL)
        epoxy_add_test (wgl_core_and_exts "${EPOXY_WGL_TESTS_COMMON};wgl_core_and_exts.c")
		epoxy_add_test (wgl_per_context_funcptrs "${EPOXY_WGL_TESTS_COMMON};wgl_per_context_funcptrs.c")
		epoxy_add_test (wgl_usefontbitmaps_ascii "${EPOXY_WGL_TESTS_COMMON};wgl_usefontbitmaps.c")
		epoxy_add_test (wgl_usefontbitmaps_unicode "${EPOXY_WGL_TESTS_COMMON};wgl_usefontbitmaps.c"
                                "" "_UNICODE;UNICODE")
	endif ()

endif ()
