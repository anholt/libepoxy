set (EPOXY_APIS gl egl wgl glx)
if (EPOXY_REBUILD_FROM_SPECS)
    set (EPOXY_GENERATED_FILES "")
    foreach (EPOXY_API ${EPOXY_APIS})
        set (EPOXY_API_GENERATED_FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${EPOXY_API}_generated_dispatch.c"
          "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/${EPOXY_API}_generated.h")
        list (APPEND EPOXY_GENERATED_FILES ${EPOXY_API_GENERATED_FILES})
        add_custom_command (
          OUTPUT ${EPOXY_API_GENERATED_FILES}
          COMMAND "${PYTHON_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/gen_dispatch.py" --dir ..
            "${CMAKE_CURRENT_SOURCE_DIR}/../registry/${EPOXY_API}.xml"
          MAIN_DEPENDENCY
            "../registry/${EPOXY_API}.xml"
          DEPENDS
            gen_dispatch.py)
    endforeach ()
    add_custom_target (epoxy_generated ALL SOURCES ${EPOXY_GENERATED_FILES})
else ()
    foreach (EPOXY_API ${EPOXY_APIS})
        add_custom_command (
          OUTPUT
            "${EPOXY_API}_generated_dispatch.c"
          COMMAND "${CMAKE_COMMAND}" -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${EPOXY_API}_pregenerated_dispatch.c"
            "${CMAKE_CURRENT_BINARY_DIR}/${EPOXY_API}_generated_dispatch.c"
          MAIN_DEPENDENCY
            "${CMAKE_CURRENT_SOURCE_DIR}/${EPOXY_API}_pregenerated_dispatch.c")
        add_custom_command (
          OUTPUT
            "../include/epoxy/${EPOXY_API}_generated.h"
          COMMAND "${CMAKE_COMMAND}" -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/../include/epoxy/${EPOXY_API}_pregenerated.h"
            "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/${EPOXY_API}_generated.h"
          MAIN_DEPENDENCY
            "${CMAKE_CURRENT_SOURCE_DIR}/../include/epoxy/${EPOXY_API}_pregenerated.h")
    endforeach ()
endif ()

set (HEADERS "../include/epoxy/common.h"
             "../include/epoxy/gl.h"
             "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/gl_generated.h"
             "../include/epoxy/khrplatform.h")
set (SOURCES dispatch_common.c "${CMAKE_CURRENT_BINARY_DIR}/gl_generated_dispatch.c")
list (APPEND HEADERS "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/gl_generated.h"
                     "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/config.h")

if (EPOXY_SUPPORT_EGL)
    list (APPEND SOURCES dispatch_egl.c "${CMAKE_CURRENT_BINARY_DIR}/egl_generated_dispatch.c")
    list (APPEND HEADERS "../include/epoxy/egl.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/egl_generated.h"
                         "../include/epoxy/eglplatform.h")
endif ()
if (EPOXY_SUPPORT_GLX)
    list (APPEND SOURCES dispatch_glx.c "${CMAKE_CURRENT_BINARY_DIR}/glx_generated_dispatch.c")
    list (APPEND HEADERS "../include/epoxy/glx.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/glx_generated.h")
endif ()
if (EPOXY_SUPPORT_WGL)
    list (APPEND SOURCES dispatch_wgl.c "${CMAKE_CURRENT_BINARY_DIR}/wgl_generated_dispatch.c")
    list (APPEND HEADERS "../include/epoxy/wgl.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/../include/epoxy/wgl_generated.h")
endif ()

add_library ("${TARGET_NAME}" SHARED ${SOURCES} ${HEADERS})
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions("${TARGET_NAME}" PUBLIC "inline=__inline")
endif()
set_target_properties("${TARGET_NAME}" PROPERTIES
                        DEFINE_SYMBOL  EPOXY_EXPORTS)
if (NOT ANDROID)
    set_target_properties("${TARGET_NAME}" PROPERTIES
                            VERSION    "1.3.1_yaronct-2"
                            SOVERSION  "${EPOXY_ABI_VER}")
endif ()

target_include_directories ("${TARGET_NAME}" PUBLIC
                              "../include"
                              "${CMAKE_CURRENT_BINARY_DIR}/../include"
                              "${CMAKE_CURRENT_SOURCE_DIR}")
install (FILES ${HEADERS} DESTINATION "include/epoxy")
install (TARGETS "${TARGET_NAME}"
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    INSTALL (FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/${TARGET_NAME}.pdb"
             DESTINATION lib  CONFIGURATIONS Debug)
endif ()
