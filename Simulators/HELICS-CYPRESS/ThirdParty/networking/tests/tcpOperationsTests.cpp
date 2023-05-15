/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <stdlib.h>
#include <thread>

#include "gmlc/networking/AsioContextManager.h"
#include "gmlc/networking/TcpOperations.h"
#include "gmlc/networking/addressOperations.hpp"
#include "gmlc/networking/interfaceOperations.hpp"
using namespace gmlc::networking;

void handler(const std::error_code& /*e*/, std::size_t bytes_transferred)
{
    CHECK(bytes_transferred == 5);
}

void client(TcpConnection::pointer cpt)
{
    std::string s = "test0";
    const std::size_t dataSize = s.size();
    char* data = new char[dataSize];
    strcpy(data, s.c_str());
    cpt->send_async(data, dataSize, handler);
}

TEST_CASE("asynchronousTcpOperationsTest", "[TcpOps]")
{
    auto io_context_server =
        gmlc::networking::AsioContextManager::getContextPointer(
            "io_context_server");

    auto server_context_loop = io_context_server->startContextLoop();
    auto spt = TcpServer::create(
        io_context_server->getBaseContext(), "localhost", 19888, true);
    int itCount{0};
    while (!spt->isReady()) {
        ++itCount;
        if (itCount > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    REQUIRE(spt->isReady());

    size_t data_recv_size;
    spt->setDataCall([&](const gmlc::networking::TcpConnection::pointer&,
                         const char* data,
                         size_t datasize) {
        CHECK(datasize == 5);
        data_recv_size = datasize;
        CHECK(std::string(data, datasize) == "test0");
        return datasize;
    });
    spt->setErrorCall([](const gmlc::networking::TcpConnection::pointer&,
                         const std::error_code& error) {
        INFO("Error (" << error.value() << "): " << error.message());
        CHECK(false);
        return false;
    });

    spt->setLoggingFunction([](int loglevel, const std::string& logMessage) {
        INFO(logMessage << " loglevel=" << loglevel);
    });

    spt->start();

    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0);
    auto cpt = establishConnection(
        io_context_server->getBaseContext(),
        std::string("localhost"),
        "19888",
        timeOut);
    itCount = 0;
    while (!cpt->isConnected()) {
        ++itCount;
        if (itCount > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    REQUIRE(cpt->isConnected());
    std::thread c(client, std::ref(cpt));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    c.join();

    spt->close();
    cpt->close();

    CHECK(data_recv_size == 5);
}

TEST_CASE("TcpOperationsTest", "[TcpOps]")
{
    auto io_context_server =
        gmlc::networking::AsioContextManager::getContextPointer(
            "io_context_server");

    auto server_context_loop = io_context_server->startContextLoop();
    auto spt = TcpServer::create(
        io_context_server->getBaseContext(), "*", 19888, true);
    int itCount{0};
    while (!spt->isReady()) {
        ++itCount;
        if (itCount > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    REQUIRE(spt->isReady());

    size_t data_recv_size;
    spt->setDataCall([&](const gmlc::networking::TcpConnection::pointer&,
                         const char* data,
                         size_t datasize) {
        CHECK(datasize == 5);
        data_recv_size = datasize;
        CHECK(std::string(data, datasize) == "test0");
        return datasize;
    });
    spt->setErrorCall([](const gmlc::networking::TcpConnection::pointer&,
                         const std::error_code& error) {
        INFO("Error (" << error.value() << "): " << error.message());
        CHECK(false);
        return false;
    });

    spt->setLoggingFunction([](int loglevel, const std::string& logMessage) {
        INFO(logMessage << " loglevel=" << loglevel);
    });

    spt->start();

    auto io_context_client =
        gmlc::networking::AsioContextManager::getContextPointer(
            "io_context_client");
    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0);
    auto cpt = establishConnection(
        io_context_client->getBaseContext(),
        std::string("localhost"),
        "19888",
        timeOut);
    auto client_ctxt_loop = io_context_client->startContextLoop();
    itCount = 0;
    while (!cpt->isConnected()) {
        ++itCount;
        if (itCount > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    REQUIRE(cpt->isConnected());
    cpt->send("test0");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    spt->close();
    cpt->close();

    CHECK(data_recv_size == 5);
}
