/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gmlc/networking/AsioContextManager.h"
#include "gmlc/networking/SocketFactory.h"
#include "gmlc/networking/TcpConnection.h"
#include "gmlc/networking/TcpServer.h"
#include <chrono>
#include <string>
#include <thread>

#ifndef INFO
#define INFO(arg)
#endif
/** test case for establishing and sending data over an unencrypted connection,
 * using settings parsed from a JSON config string*/
TEST_CASE("simple_comm_test", "[simpleConnections]")
{
    // Create a socket factory using a JSON config string
    gmlc::networking::SocketFactory sf(R"({"encrypted": false})", false);

    auto ioctx_server =
        gmlc::networking::AsioContextManager::getContextPointer("server");
    auto server = gmlc::networking::TcpServer::create(
        sf, ioctx_server->getBaseContext(), "*", 10101, true);
    while (!server->isReady()) {
    }
    INFO("Server ready");
    auto ctxloop_server = ioctx_server->startContextLoop();

    // Data callback that checks if data received is what was expected
    size_t data_recv_size;
    server->setDataCall(
        [&](const gmlc::networking::TcpConnection::pointer& /*connection*/,
            const char* data,
            size_t datasize) {
            CHECK(datasize == 4);
            data_recv_size = datasize;
            CHECK(std::string(data, datasize) == "test");
            return datasize;
        });
    server->setErrorCall(
        [](const gmlc::networking::TcpConnection::pointer& /*connection*/,
           const std::error_code& error) {
            INFO("Error (" << error.value() << "): " << error.message());
            CHECK(false);
            return false;
        });

    /*
        // This doesn't seem to work for redirecting logging messages to catch2
        server->setLoggingFunction(
            [](int loglevel, const std::string& logMessage) {
                INFO(logMessage << " loglevel=" << loglevel);
            });
    */

    INFO("Server starting...");
    CHECK(server->start());
    INFO("Server started");

    INFO("Creating connection...");
    auto ioctx_client =
        gmlc::networking::AsioContextManager::getContextPointer("client");
    auto connection = gmlc::networking::TcpConnection::create(
        sf, ioctx_client->getBaseContext(), "127.0.0.1", "10101");
    auto ctxloop_client = ioctx_client->startContextLoop();
    INFO("Connection created");
    while (!connection->isConnected()) {
    }
    INFO("Connection established")

    INFO("Sending test string");
    connection->send("test");

    // Sleep to give time for the client/server threads to run
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    INFO("Shutdown server")
    server->close();

    // One last check to make sure the data receive callback actually ran
    INFO("Data size: " << data_recv_size);
    CHECK(data_recv_size == 4);
}

#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
/** test case for establishing and sending data over an encrypted connection,
 * using settings loaded from a JSON config file*/
TEST_CASE("simple_encrypted_comm_test", "[simpleConnections]")
{
    // Create a SocketFactory using values from a JSON config file
    gmlc::networking::SocketFactory sf(
        std::string(TEST_BINDIR) + "/test_files/ssl_encryption_config.json");

    auto ioctx_server =
        gmlc::networking::AsioContextManager::getContextPointer("server");
    auto server = gmlc::networking::TcpServer::create(
        sf, ioctx_server->getBaseContext(), "*", 10101, true);
    while (!server->isReady()) {
    }
    INFO("Server ready");
    auto ctxloop_server = ioctx_server->startContextLoop();

    // Data callback that checks if data received is what was expected
    size_t data_recv_size;
    server->setDataCall(
        [&](const gmlc::networking::TcpConnection::pointer& /*connection*/,
            const char* data,
            size_t datasize) {
            CHECK(datasize == 4);
            data_recv_size = datasize;
            CHECK(std::string(data, datasize) == "test");
            return datasize;
        });
    server->setErrorCall(
        [](const gmlc::networking::TcpConnection::pointer& /*connection*/,
           const std::error_code& error) {
            INFO("Error (" << error.value() << "): " << error.message());
            CHECK(false);
            return false;
        });

    INFO("Server starting...");
    CHECK(server->start());
    INFO("Server started");

    INFO("Creating connection...");
    // Separate client ioctx to prevent race between blocking SSL handshake
    // functions
    auto ioctx_client =
        gmlc::networking::AsioContextManager::getContextPointer("client");
    auto connection = gmlc::networking::TcpConnection::create(
        sf, ioctx_client->getBaseContext(), "127.0.0.1", "10101");
    auto ctxloop_client = ioctx_client->startContextLoop();
    INFO("Connection created");
    while (!connection->isConnected()) {
    }
    INFO("Connection established")

    // Sleep to make sure the OpenSSL handshake is finished
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    INFO("Sending test string");
    connection->send("test");

    // Sleep to give time for the client/server threads to run
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    INFO("Shutdown server")
    server->close();

    // One last check to make sure the data receive callback actually ran
    INFO("Data size: " << data_recv_size);
    CHECK(data_recv_size == 4);
}
#endif
