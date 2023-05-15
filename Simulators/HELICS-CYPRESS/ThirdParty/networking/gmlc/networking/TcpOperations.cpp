/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#include "TcpOperations.h"

#include "AsioContextManager.h"

#include "TcpHelperClasses.h"
#include "addressOperations.hpp"
#include <memory>
#include <string>
#include <thread>

namespace gmlc::networking {

TcpConnection::pointer establishConnection(
    asio::io_context& io_context,
    const std::string& host,
    const std::string& port,
    std::chrono::milliseconds timeOut)
{
    return establishConnection(
        SocketFactory(), io_context, host, port, timeOut);
}

TcpConnection::pointer establishConnection(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& host,
    const std::string& port,
    std::chrono::milliseconds timeOut)
{
    using std::chrono::milliseconds;
    using std::chrono::steady_clock;

    using namespace std::chrono_literals;  // NOLINT

    TcpConnection::pointer connectionPtr;
    connectionPtr = TcpConnection::create(sf, io_context, host, port);
    if (timeOut <= std::chrono::milliseconds(0)) {
        return connectionPtr;
    }

    auto tick = steady_clock::now();
    milliseconds timeRemaining(timeOut);
    milliseconds timeRemPrev(timeOut);
    int trycnt = 1;
    while (!connectionPtr->waitUntilConnected(timeRemaining)) {
        auto tock = steady_clock::now();
        timeRemaining = milliseconds(timeOut) -
            std::chrono::duration_cast<milliseconds>(tock - tick);
        if ((timeRemaining < 0ms) && (trycnt > 1)) {
            connectionPtr = nullptr;
            break;
        }
        // make sure we slow down and sleep for a little bit
        if (timeRemPrev - timeRemaining < 100ms) {
            std::this_thread::sleep_for(200ms);
        }
        timeRemPrev = timeRemaining;
        if (timeRemaining < 0ms) {
            timeRemaining = 400ms;
        }

        // lets try to connect again
        ++trycnt;
        connectionPtr = TcpConnection::create(sf, io_context, host, port);
    }
    return connectionPtr;
}

TcpConnection::pointer establishConnection(
    asio::io_context& io_context,
    const std::string& address,
    std::chrono::milliseconds timeOut)
{
    return establishConnection(SocketFactory(), io_context, address, timeOut);
}

TcpConnection::pointer establishConnection(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    std::chrono::milliseconds timeOut)
{
    std::string interface;
    std::string port;
    std::tie(interface, port) = extractInterfaceAndPortString(address);
    return establishConnection(sf, io_context, interface, port, timeOut);
}
}  // namespace gmlc::networking
