# ~~~
# Copyright (c) 2017-2022, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~

cmake_minimum_required(VERSION 3.11...3.23)

project(
    GMLC_UTILITIES
    LANGUAGES CXX
    VERSION 0.4.0
)

# -----------------------------------------------------------------------------
# GMLC UTILITIES library Version number
# -----------------------------------------------------------------------------
set(GMLC_UTILITIES_VERSION_BUILD)
set(GMLC_UTILITIES_DATE "2022-07-04")

set(GMLC_UTILITIES_VERSION_STRING "${GMLC_UTILITIES_VERSION} (${GMLC_UTILITIES_DATE})")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
# -----------------------------------------------------------------------------
# set the module path and include some common macros
# -----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/config/cmake/")
include(extraMacros)
include(CMakeDependentOption)
include(CTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Prohibit in-source build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(
        FATAL_ERROR
            "In-source build is not supported. Please, use an empty directory for building the project."
    )
endif()

option(GMLC_UTILITIES_WITH_CMAKE_PACKAGE "Generate and install cmake package files"
       ${UTILITIES_MASTER_PROJECT}
)
mark_as_advanced(GMLC_UTILITIES_WITH_CMAKE_PACKAGE)

# Install instructions for this target
if(GMLC_UTILITIES_WITH_CMAKE_PACKAGE)
    set(UTILITIES_LIBRARY_EXPORT_COMMAND EXPORT utilitiesConfig)
else(GMLC_UTILITIES_WITH_CMAKE_PACKAGE)
    set(UTILITIES_LIBRARY_EXPORT_COMMAND)
endif(GMLC_UTILITIES_WITH_CMAKE_PACKAGE)

cmake_dependent_option(
    GMLC_UTILITIES_CLANG_TIDY "Look for and use Clang-Tidy" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME;NOT CMAKE_VERSION VERSION_LESS 3.6" OFF
)
set(GMLC_UTILITIES_CLANG_TIDY_OPTIONS
    ""
    CACHE STRING "Clang tidy options, such as -fix, semicolon separated"
)

mark_as_advanced(GMLC_UTILITIES_CLANG_TIDY_OPTIONS)
mark_as_advanced(GMLC_UTILITIES_CLANG_TIDY)

add_library(utilities_base INTERFACE)

if(NOT TARGET compile_flags_target)
    add_library(compile_flags_target INTERFACE)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(compiler_flags)
    message(STATUS "setting version build options to \"${CXX_STANDARD_FLAG}\"")
    if(CXX_STANDARD_FLAG)
        if(MSVC)
            target_compile_options(utilities_base INTERFACE ${CXX_STANDARD_FLAG})
        else()
            target_compile_options(
                utilities_base
                INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${CXX_STANDARD_FLAG}>
            )
        endif(MSVC)
    endif(CXX_STANDARD_FLAG)
endif()

target_link_libraries(utilities_base INTERFACE compile_flags_target)

option(GMLC_UTILITIES_INSTALL "Enable GMLC utilities to be installed" ON)
option(GMLC_UTILITIES_INCLUDE_BOOST "Enable some boost library headers to be used" ON)
option(GMLC_UTILITIES_USE_BOOST_SPIRIT
       "Enable the use of the boost::spirit parser for converting strings" ON
)
mark_as_advanced(GMLC_UTILITIES_USE_BOOST_SPIRIT)

# Add boost to test boost::optional if available
if(GMLC_UTILITIES_INCLUDE_BOOST)
    find_package(Boost 1.69)
    if(Boost_FOUND)
        message(STATUS "Boost dir= ${Boost_INCLUDE_DIRS}")
        target_include_directories(
            utilities_base SYSTEM INTERFACE $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        )

        if(GMLC_UTILITIES_USE_BOOST_SPIRIT)
            target_compile_definitions(utilities_base INTERFACE -DUSE_BOOST_SPIRIT=1)
        endif()

    endif()
endif()

# Prepare Clang-Tidy
if(GMLC_UTILITIES_CLANG_TIDY)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable" REQUIRED
    )

    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" ${GMLC_UTILITIES_CLANG_TIDY_OPTIONS})
endif()

if(GMLC_UTILITIES_INSTALL)
    install(TARGETS utilities_base ${UTILITIES_LIBRARY_EXPORT_COMMAND})
endif()

cmake_dependent_option(
    GMLC_UTILITIES_TEST "Enable tests for the containers library" ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

cmake_dependent_option(
    GMLC_UTILITIES_OBJECT_LIB "Enable construction of the utilities object library" OFF
    "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

cmake_dependent_option(
    GMLC_UTILITIES_STATIC_LIB "Enable construction of the utilities static library" ON
    "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" ON
)

add_subdirectory(gmlc)

if(GMLC_UTILITIES_TEST AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

# -------------------------------------------------------------
# Get some configuration for C++17 as that becomes available
# -------------------------------------------------------------
# message(STATUS ${CMAKE_CXX_FLAGS})

cmake_dependent_option(
    GMLC_UTILITIES_GENERATE_DOXYGEN_DOC "Generate Doxygen doc target" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

if(GMLC_UTILITIES_GENERATE_DOXYGEN_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)

        show_variable(
            GMLC_UTILITIES_DOXYGEN_OUTPUT_DIR PATH "location to put Doxygen docs"
            "${PROJECT_BINARY_DIR}/docs"
        )
        configure_file(
            ${PROJECT_SOURCE_DIR}/config/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
        )
        add_custom_target(
            doc
            ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${DOXYGET_OUTPUT_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif(DOXYGEN_FOUND)
endif(GMLC_UTILITIES_GENERATE_DOXYGEN_DOC)
