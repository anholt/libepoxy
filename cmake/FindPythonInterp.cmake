#.rst:
# FindPythonInterp
# ----------------
#
# Find python interpreter
#
# This module finds if Python interpreter is installed and determines
# where the executables are.  This code sets the following variables:
#
# ::
#
#   PYTHONINTERP_FOUND         - Was the Python executable found
#   PYTHON_EXECUTABLE          - path to the Python interpreter
#
#
#
# ::
#
#   PYTHON_VERSION_STRING      - Python version found e.g. 2.5.2
#   PYTHON_VERSION_MAJOR       - Python major version found e.g. 2
#   PYTHON_VERSION_MINOR       - Python minor version found e.g. 5
#   PYTHON_VERSION_PATCH       - Python patch version found e.g. 2
#
#
#
# The Python_ADDITIONAL_VERSIONS variable can be used to specify a list
# of version numbers that should be taken into account when searching
# for Python.  You need to set this variable before calling
# find_package(PythonInterp).
#
# If also calling find_package(PythonLibs), call find_package(PythonInterp)
# first to get the currently active Python version by default with a consistent
# version of PYTHON_LIBRARIES.

#=============================================================================
# Copyright 2005-2010 Kitware, Inc.
# Copyright 2011 Bjoern Ricks <bjoern.ricks@gmail.com>
# Copyright 2012 Rolf Eike Beer <eike@sf-mail.de>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set (_Python_VERSIONS 3 3.4 3.3 3.2 3.1 3.0 2 2.7 2.6 2.5 2.4 2.3 2.2 2.1 2.0 "")

foreach (_CURRENT_VERSION IN LISTS _Python_VERSIONS)
    set (_Python_NAMES python${_CURRENT_VERSION})
    find_program (PYTHON_EXECUTABLE
        NAMES ${_Python_NAMES}
        PATHS [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath])
endforeach ()

# determine python version string
if(PYTHON_EXECUTABLE)
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c
                            "import sys; sys.stdout.write(';'.join([str(x) for x in sys.version_info[:3]]))"
                    OUTPUT_VARIABLE _VERSION
                    RESULT_VARIABLE _PYTHON_VERSION_RESULT
                    ERROR_QUIET)
    if(NOT _PYTHON_VERSION_RESULT)
        string(REPLACE ";" "." PYTHON_VERSION_STRING "${_VERSION}")
        list(GET _VERSION 0 PYTHON_VERSION_MAJOR)
        list(GET _VERSION 1 PYTHON_VERSION_MINOR)
        list(GET _VERSION 2 PYTHON_VERSION_PATCH)
        if(PYTHON_VERSION_PATCH EQUAL 0)
            # it's called "Python 2.7", not "2.7.0"
            string(REGEX REPLACE "\\.0$" "" PYTHON_VERSION_STRING "${PYTHON_VERSION_STRING}")
        endif()
    else()
        # sys.version predates sys.version_info, so use that
        execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c "import sys; sys.stdout.write(sys.version)"
                        OUTPUT_VARIABLE _VERSION
                        RESULT_VARIABLE _PYTHON_VERSION_RESULT
                        ERROR_QUIET)
        if(NOT _PYTHON_VERSION_RESULT)
            string(REGEX REPLACE " .*" "" PYTHON_VERSION_STRING "${_VERSION}")
            string(REGEX REPLACE "^([0-9]+)\\.[0-9]+.*" "\\1" PYTHON_VERSION_MAJOR "${PYTHON_VERSION_STRING}")
            string(REGEX REPLACE "^[0-9]+\\.([0-9])+.*" "\\1" PYTHON_VERSION_MINOR "${PYTHON_VERSION_STRING}")
            if(PYTHON_VERSION_STRING MATCHES "^[0-9]+\\.[0-9]+\\.([0-9]+)")
                set(PYTHON_VERSION_PATCH "${CMAKE_MATCH_1}")
            else()
                set(PYTHON_VERSION_PATCH "0")
            endif()
        else()
            # sys.version was first documented for Python 1.5, so assume
            # this is older.
            set(PYTHON_VERSION_STRING "1.4")
            set(PYTHON_VERSION_MAJOR "1")
            set(PYTHON_VERSION_MINOR "4")
            set(PYTHON_VERSION_PATCH "0")
        endif()
    endif()
    unset(_PYTHON_VERSION_RESULT)
    unset(_VERSION)
endif()

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (PythonInterp REQUIRED_VARS PYTHON_EXECUTABLE VERSION_VAR PYTHON_VERSION_STRING)

mark_as_advanced (PYTHON_EXECUTABLE)
