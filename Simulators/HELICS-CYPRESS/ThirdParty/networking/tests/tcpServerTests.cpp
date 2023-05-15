/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gmlc/networking/TcpOperations.h"
#include "gmlc/networking/addressOperations.hpp"
#include "gmlc/networking/interfaceOperations.hpp"
using namespace gmlc::networking;

TEST_CASE("localHost", "[TcpServer]")
{
    asio::io_context io_context;
    auto localhost = "127.0.0.1";
    CHECK_NOTHROW(TcpServer::create(io_context, localhost, "0", false));
}

TEST_CASE("externalAddressV4", "[TcpServer]")
{
    asio::io_context io_context;
    CHECK_NOTHROW(
        TcpServer::create(io_context, getLocalExternalAddressV4(), "0", false));
}

TEST_CASE("localHostString", "[TcpServer]")
{
    asio::io_context io_context;
    CHECK_NOTHROW(TcpServer::create(io_context, "localhost", "0", false));
}
TEST_CASE("invalidString", "[TcpServer]")
{
    asio::io_context io_context;
    CHECK_THROWS(TcpServer::create(io_context, "testString", "0", false));
}
