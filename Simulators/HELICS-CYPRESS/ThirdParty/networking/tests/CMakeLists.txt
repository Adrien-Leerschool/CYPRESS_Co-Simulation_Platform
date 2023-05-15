# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2021, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(NETWORKING_TESTS addressOperationsTests interfaceOperationsTests)

if(NOT GMLC_NETWORKING_DISABLE_ASIO)
    list(
        APPEND
        NETWORKING_TESTS
        simpleConnectionTests
        tcpOperationsTests
        tcpServerTests
        tcpClientTests
        contextManagerTests
    )
endif()

if(GMLC_NETWORKING_ENABLE_ENCRYPTION)
    configure_file(
        "test_files/ssl_encryption_config.json.in"
        "test_files/ssl_encryption_config.json"
    )
endif()

foreach(T ${NETWORKING_TESTS})

    add_executable(${T} ${T}.cpp)
    target_link_libraries(${T} PUBLIC gmlc::networking)
    add_test(${T} ${T})
    set_target_properties(${T} PROPERTIES FOLDER tests)
    if(GMLC_NETWORKING_CLANG_TIDY)
        set_property(TARGET ${T} PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
    endif()
    target_compile_definitions(
        ${T} PRIVATE "TEST_BINDIR=\"${CMAKE_CURRENT_BINARY_DIR}\""
    )
    target_include_directories(${T} PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty/catch2)
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    include(CodeCoverage)
    setup_target_for_coverage(
        NAME NETWORKING_coverage EXECUTABLE ctest DEPENDENCIES ${NETWORKING_TESTS}
    )
endif()
