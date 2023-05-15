# ~~~
# Copyright (c) 2017-2021, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~

# Make sure users don't get warnings on a tested (3.0 to 3.17) version of CMake. For
# most of the policies, the new version is better (hence the change). We don't use the
# 3.0...3.16 syntax because of a bug in an older MSVC's built-in and modified CMake 3.11

cmake_minimum_required(VERSION 3.10...3.22)

project(
    GMLC_NETWORKING
    LANGUAGES CXX
    VERSION 0.1.0
)

# -----------------------------------------------------------------------------
# GMLC NETWORKING library Version number
# -----------------------------------------------------------------------------
set(GMLC_NETWORKING_VERSION_BUILD)
set(GMLC_NETWORKING_DATE "12-02-2021")

set(GMLC_NETWORKING_VERSION_STRING
    "${GMLC_NETWORKING_VERSION} (${GMLC_NETWORKING_DATE})"
)

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
list(APPEND CMAKE_MODULE_PATH "${GMLC_NETWORKING_SOURCE_DIR}/config")
list(APPEND CMAKE_MODULE_PATH "${GMLC_NETWORKING_SOURCE_DIR}/ThirdParty/cmake")
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

option(GMLC_NETWORKING_WITH_CMAKE_PACKAGE "Generate and install cmake package files"
       ${GMLC_NETWORKING_MASTER_PROJECT}
)
mark_as_advanced(GMLC_NETWORKING_WITH_CMAKE_PACKAGE)

# Install instructions for this target
if(GMLC_NETWORKING_WITH_CMAKE_PACKAGE)
    set(GMLC_NETWORKING_LIBRARY_EXPORT_COMMAND EXPORT networkingConfig)
else(GMLC_NETWORKING_WITH_CMAKE_PACKAGE)
    set(GMLC_NETWORKING_LIBRARY_EXPORT_COMMAND)
endif(GMLC_NETWORKING_WITH_CMAKE_PACKAGE)

cmake_dependent_option(
    GMLC_NETWORKING_CLANG_TIDY "Look for and use Clang-Tidy" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)
set(GMLC_NETWORKING_CLANG_TIDY_OPTIONS
    ""
    CACHE STRING "Clang tidy options, such as -fix, semicolon separated"
)

mark_as_advanced(GMLC_NETWORKING_CLANG_TIDY_OPTIONS)
mark_as_advanced(GMLC_NETWORKING_CLANG_TIDY)

add_library(networking_base INTERFACE)

if(NOT TARGET compile_flags_target)
    add_library(compile_flags_target INTERFACE)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(compiler_flags)
    message(STATUS "setting version build options to \"${CXX_STANDARD_FLAG}\"")
    if(CXX_STANDARD_FLAG)
        if(MSVC)
            target_compile_options(networking_base INTERFACE ${CXX_STANDARD_FLAG})
        else()
            target_compile_options(
                networking_base
                INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${CXX_STANDARD_FLAG}>
            )
        endif(MSVC)
    endif(CXX_STANDARD_FLAG)
endif()

target_link_libraries(networking_base INTERFACE compile_flags_target)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(networking_base INTERFACE Threads::Threads)

# MinGW/MSYS/Cygwin support
if(MINGW
   OR MSYS
   OR CYGWIN
)
    target_link_libraries(networking_base INTERFACE wsock32 ws2_32 mswsock iphlpapi)
    if(NOT MSYS)
        target_compile_definitions(networking_base INTERFACE -D_WIN32_WINNT=0x0601)
    endif()
    if(CYGWIN)
        target_compile_definitions(
            networking_base INTERFACE -D_XOPEN_SOURCE=500 -D__USE_W32_SOCKETS
        )
    endif()
endif()

# -------------------------------------------------------------
# global include directories
# -------------------------------------------------------------

include(updateGitSubmodules)

cmake_dependent_option(
    GMLC_NETWORKING_DISABLE_ASIO
    "Disable parts of the library that use ASIO (most of it)" OFF "NOT CYGWIN" ON
)
cmake_dependent_option(
    GMLC_NETWORKING_ENABLE_ENCRYPTION
    "Enable OpenSSL support in ASIO for encrypted communication" OFF
    "NOT GMLC_NETWORKING_DISABLE_ASIO" OFF
)
if(NOT GMLC_NETWORKING_DISABLE_ASIO)
    target_compile_definitions(networking_base INTERFACE "-DASIO_STANDALONE")
    if(NOT GMLC_NETWORKING_ASIO_INCLUDE)
        set(GMLC_NETWORKING_ASIO_INCLUDE
            ${PROJECT_SOURCE_DIR}/ThirdParty/asio/asio/include
        )
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ThirdParty/asio/asio/include/asio.hpp")
            submod_update(ThirdParty/asio)
        endif()
    endif()

    target_include_directories(
        networking_base SYSTEM
        INTERFACE $<BUILD_INTERFACE:${GMLC_NETWORKING_ASIO_INCLUDE}>
    )

    # Find OpenSSL
    if(GMLC_NETWORKING_ENABLE_ENCRYPTION)
        find_package(OpenSSL REQUIRED)
        target_link_libraries(networking_base INTERFACE OpenSSL::SSL OpenSSL::Crypto)
        if(TARGET OpenSSL::applink)
            target_link_libraries(networking_base INTERFACE OpenSSL::applink)
        endif()
    endif()
endif()

if(NOT GMLC_NETWORKING_CONCURRENCY_INCLUDE)
    set(GMLC_NETWORKING_CONCURRENCY_INCLUDE
        ${PROJECT_SOURCE_DIR}/ThirdParty/concurrency
    )
    if(NOT EXISTS
       "${PROJECT_SOURCE_DIR}/ThirdParty/concurrency/gmlc/concurrency/Barrier.hpp"
    )
        submod_update(ThirdParty/concurrency)
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ThirdParty/NetIF/gmlc/netif/NetIF.hpp")
    submod_update(ThirdParty/NetIF)
endif()

target_include_directories(
    networking_base INTERFACE $<BUILD_INTERFACE:${GMLC_NETWORKING_CONCURRENCY_INCLUDE}>
                              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/ThirdParty/NetIF>
)

option(GMLC_NETWORKING_USE_EXTERNAL_JSON "Use an external nlohmann_json library" OFF)
if(GMLC_NETWORKING_USE_EXTERNAL_JSON)
    find_package(nlohmann_json 3.10 REQUIRED)
else()
    set(JSON_BuildTests
        OFF
        CACHE INTERNAL ""
    )
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ThirdParty/nlohmann_json/CMakeLists.txt")
        submod_update(ThirdParty/nlohmann_json)
    endif()
    add_subdirectory(ThirdParty/nlohmann_json)
endif()

option(GMLC_NETWORKING_INSTALL "Enable GMLC utilities to be installed" ON)
option(GMLC_NETWORKING_INCLUDE_BOOST "Enable some boost library headers to be used" OFF)

# Add boost to test boost::optional if available
if(GMLC_NETWORKING_INCLUDE_BOOST)
    find_package(Boost 1.67)
    if(Boost_FOUND)
        message(STATUS "Boost dir= ${Boost_INCLUDE_DIRS}")
        target_include_directories(
            networking_base SYSTEM INTERFACE $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        )

    endif()
endif()

# Prepare Clang-Tidy
if(GMLC_NETWORKING_CLANG_TIDY)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable" REQUIRED
    )

    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" ${GMLC_NETWORKING_CLANG_TIDY_OPTIONS})
endif()

if(GMLC_NETWORKING_INSTALL)
    install(TARGETS networking_base ${GMLC_NETWORKING_LIBRARY_EXPORT_COMMAND})
endif()

cmake_dependent_option(
    GMLC_NETWORKING_TEST "Enable tests for the containers library" ON
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

cmake_dependent_option(
    GMLC_NETWORKING_OBJECT_LIB "Enable construction of the utilities object library"
    OFF "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

cmake_dependent_option(
    GMLC_NETWORKING_STATIC_LIB "Enable construction of the utilities static library" ON
    "NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" ON
)

add_subdirectory(gmlc)

if(GMLC_NETWORKING_TEST AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

# message(STATUS ${CMAKE_CXX_FLAGS})

cmake_dependent_option(
    GMLC_NETWORKING_GENERATE_DOXYGEN_DOC "Generate Doxygen doc target" OFF
    "CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME" OFF
)

if(GMLC_NETWORKING_GENERATE_DOXYGEN_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)

        show_variable(
            GMLC_NETWORKING_DOXYGEN_OUTPUT_DIR PATH "location to put Doxygen docs"
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
endif(GMLC_NETWORKING_GENERATE_DOXYGEN_DOC)
