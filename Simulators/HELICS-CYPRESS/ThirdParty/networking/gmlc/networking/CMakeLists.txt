# ~~~
# Copyright (c) 2017-2021, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~

set(networking_nonasio_source_files addressOperations.cpp interfaceOperations.cpp)

set(networking_asio_source_files
    AsioContextManager.cpp SocketFactory.cpp TcpOperations.cpp TcpConnection.cpp
    TcpServer.cpp TcpAcceptor.cpp
)

set(networking_nonasio_header_files GuardedTypes.hpp addressOperations.hpp
                                    interfaceOperations.hpp
)

set(networking_asio_header_files
    TcpOperations.h
    AsioContextManager.h
    TcpHelperClasses.h
    TcpConnection.h
    TcpServer.h
    TcpAcceptor.h
    Socket.h
    SocketFactory.h
)

if(GMLC_NETWORKING_OBJECT_LIB)
    if(GMLC_NETWORKING_DISABLE_ASIO)
        add_library(
            gmlc_networking_obj OBJECT ${networking_nonasio_source_files}
                                       ${networking_nonasio_header_files}
        )

    else()
        add_library(
            gmlc_networking_obj OBJECT
            ${networking_asio_source_files} ${networking_nonasio_source_files}
            ${networking_asio_header_files} ${networking_nonasio_header_files}
        )
    endif()
    target_include_directories(
        gmlc_networking_obj PUBLIC $<BUILD_INTERFACE:${GMLC_UTILITIES_SOURCE_DIR}>
    )
    target_include_directories(
        gmlc_networking_obj
        PRIVATE $<TARGET_PROPERTY:networking_base,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_include_directories(
        gmlc_networking_obj SYSTEM
        PRIVATE $<TARGET_PROPERTY:networking_base,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>
    )

    target_compile_definitions(
        gmlc_networking_obj
        PRIVATE $<TARGET_PROPERTY:networking_base,INTERFACE_COMPILE_DEFINITIONS>
    )
endif()

if(GMLC_NETWORKING_STATIC_LIB)

    if(GMLC_NETWORKING_DISABLE_ASIO)
        add_library(
            gmlc_networking STATIC ${networking_nonasio_source_files}
                                   ${networking_nonasio_header_files}
        )
        target_compile_definitions(
            gmlc_networking PUBLIC "-DGMLC_NETWORKING_DISABLE_ASIO=1"
        )
    else()
        add_library(
            gmlc_networking STATIC
            ${networking_asio_source_files} ${networking_nonasio_source_files}
            ${networking_asio_header_files} ${networking_nonasio_header_files}
        )
    endif()
    target_include_directories(
        gmlc_networking PUBLIC $<BUILD_INTERFACE:${GMLC_NETWORKING_SOURCE_DIR}>
    )

    add_library(gmlc::networking ALIAS gmlc_networking)

    if(GMLC_NETWORKING_ENABLE_ENCRYPTION)
        target_compile_definitions(
            gmlc_networking PUBLIC GMLC_NETWORKING_ENABLE_ENCRYPTION
        )
        target_link_libraries(gmlc_networking PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        if(TARGET OpenSSL::applink)
            target_link_libraries(gmlc_networking PRIVATE OpenSSL::applink)
        endif()
    endif()

    target_link_libraries(gmlc_networking PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(gmlc_networking PUBLIC networking_base)
    if(GMLC_NETWORKING_INSTALL)
        install(
            TARGETS gmlc_networking ${GMLC_NETWORKING_LIBRARY_EXPORT_COMMAND}
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
        if(GMLC_NETWORKING_DISABLE_ASIO)
            install(FILES ${networking_nonasio_header_files} DESTINATION include)
        else()
            install(FILES ${networking_asio_header_files}
                          ${networking_nonasio_header_files} DESTINATION include
            )
        endif()

        if(GMLC_NETWORKING_CLANG_TIDY)
            set_property(
                TARGET gmlc_networking PROPERTY CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            )
        endif()
    endif()

endif()
