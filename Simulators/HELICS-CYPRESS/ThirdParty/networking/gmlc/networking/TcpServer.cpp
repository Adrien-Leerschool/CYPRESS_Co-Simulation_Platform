/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "TcpServer.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <utility>

namespace gmlc::networking {
using asio::ip::tcp;
using namespace std::chrono_literals;  // NOLINT

TcpServer::TcpServer(
    asio::io_context& io_context,
    const std::string& address,
    uint16_t portNum,
    bool port_reuse,
    int nominalBufferSize) :
    TcpServer::TcpServer(
        SocketFactory(),
        io_context,
        address,
        portNum,
        port_reuse,
        nominalBufferSize)
{
}
TcpServer::TcpServer(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    uint16_t portNum,
    bool port_reuse,
    int nominalBufferSize) :
    ioctx(io_context),
    socket_factory(sf), bufferSize(nominalBufferSize), reuse_address(port_reuse)
{
    if ((address == "*") || (address == "tcp://*")) {
        endpoints.emplace_back(asio::ip::address_v4::any(), portNum);
        //      endpoints.emplace_back (asio::ip::address_v6::any (), portNum);
    } else if (address == "localhost") {
        endpoints.emplace_back(asio::ip::tcp::v4(), portNum);
    } else {
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(
            tcp::v4(),
            address,
            std::to_string(portNum),
            tcp::resolver::query::canonical_name);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;
        if (endpoint_iterator != end) {
            while (endpoint_iterator != end) {
                endpoints.push_back(*endpoint_iterator);
                ++endpoint_iterator;
            }
        } else {
            halted = true;
            return;
        }
    }
    initialConnect();
}

TcpServer::TcpServer(
    asio::io_context& io_context,
    const std::string& address,
    const std::string& port,
    bool port_reuse,
    int nominalBufferSize) :
    TcpServer::TcpServer(
        SocketFactory(),
        io_context,
        address,
        port,
        port_reuse,
        nominalBufferSize)
{
}
TcpServer::TcpServer(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    const std::string& port,
    bool port_reuse,
    int nominalBufferSize) :
    ioctx(io_context),
    socket_factory(sf), bufferSize(nominalBufferSize), reuse_address(port_reuse)
{
    tcp::resolver resolver(io_context);
    tcp::resolver::query query(
        tcp::v4(), address, port, tcp::resolver::query::canonical_name);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    if (endpoint_iterator != end) {
        while (endpoint_iterator != end) {
            endpoints.push_back(*endpoint_iterator);
            ++endpoint_iterator;
        }
    } else {
        halted = true;
        return;
    }
    initialConnect();
}

TcpServer::TcpServer(
    asio::io_context& io_context,
    uint16_t portNum,
    int nominalBufferSize) :
    TcpServer::TcpServer(
        SocketFactory(),
        io_context,
        portNum,
        nominalBufferSize)
{
}
TcpServer::TcpServer(
    const SocketFactory& sf,
    asio::io_context& io_context,
    uint16_t portNum,
    int nominalBufferSize) :
    ioctx(io_context),
    socket_factory(sf), bufferSize(nominalBufferSize)
{
    endpoints.emplace_back(asio::ip::tcp::v4(), portNum);
    initialConnect();
}

TcpServer::~TcpServer()
{
    try {
        close();
    }
    catch (...) {
    }
}

void TcpServer::initialConnect()
{
    if (halted.load(std::memory_order_acquire)) {
        logger(0, "previously halted server");
        return;
    }
    for (auto& ep : endpoints) {
        auto acc = TcpAcceptor::create(ioctx, ep);
        if (reuse_address) {
            acc->set_option(tcp::acceptor::reuse_address(true));
        } else {
            acc->set_option(tcp::acceptor::reuse_address(false));
        }
        acc->setAcceptCall(
            [this](TcpAcceptor::pointer accPtr, TcpConnection::pointer conn) {
                handle_accept(std::move(accPtr), std::move(conn));
            });
        if (logFunction) {
            acc->setLoggingFunction(logFunction);
        }
        acceptors.push_back(std::move(acc));
    }
    bool anyConnect = false;
    size_t connectedAcceptors = 0;
    int index = 0;
    for (auto& acc : acceptors) {
        ++index;
        if (!acc->connect()) {
            logger(
                0,
                std::string("unable to connect acceptor ") +
                    std::to_string(index) + " of " +
                    std::to_string(acceptors.size()));
            continue;
        }
        ++connectedAcceptors;
        anyConnect = true;
    }
    if (!anyConnect) {
        halted = true;
        logger(1, "halting server operation");
        return;
    }
    if (connectedAcceptors < acceptors.size()) {
        logger(
            1,
            std::string("partial connection on the server ") +
                std::to_string(connectedAcceptors) + " of " +
                std::to_string(acceptors.size()) + " were connected");
    }
}

bool TcpServer::reConnect(std::chrono::milliseconds timeOut)
{
    halted = false;
    bool partialConnect = false;
    for (auto& acc : acceptors) {
        if (!acc->isConnected()) {
            if (!acc->connect(timeOut)) {
                if (partialConnect) {
                    logger(
                        0,
                        std::string("unable to connect all acceptors on ") +
                            acc->to_string());
                } else {
                    logger(
                        0,
                        std::string("unable to connect on ") +
                            acc->to_string());
                }

                halted = true;
                continue;
            }
        }
        partialConnect = true;
    }
    if ((halted.load()) && (partialConnect)) {
        logger(0, "partial connection on acceptor");
    }
    return !halted;
}

TcpServer::pointer TcpServer::create(
    asio::io_context& io_context,
    const std::string& address,
    uint16_t portNum,
    bool reuse_port,
    int nominalBufferSize)
{
    return TcpServer::create(
        SocketFactory(),
        io_context,
        address,
        portNum,
        reuse_port,
        nominalBufferSize);
}
TcpServer::pointer TcpServer::create(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    uint16_t portNum,
    bool reuse_port,
    int nominalBufferSize)
{
    return pointer(new TcpServer(
        sf, io_context, address, portNum, reuse_port, nominalBufferSize));
}

TcpServer::pointer TcpServer::create(
    asio::io_context& io_context,
    const std::string& address,
    const std::string& port,
    bool reuse_port,
    int nominalBufferSize)
{
    return TcpServer::create(
        SocketFactory(),
        io_context,
        address,
        port,
        reuse_port,
        nominalBufferSize);
}
TcpServer::pointer TcpServer::create(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    const std::string& port,
    bool reuse_port,
    int nominalBufferSize)
{
    return pointer(new TcpServer(
        sf, io_context, address, port, reuse_port, nominalBufferSize));
}

TcpServer::pointer TcpServer::create(
    asio::io_context& io_context,
    uint16_t portNum,
    int nominalBufferSize)
{
    return TcpServer::create(
        SocketFactory(), io_context, portNum, nominalBufferSize);
}
TcpServer::pointer TcpServer::create(
    const SocketFactory& sf,
    asio::io_context& io_context,
    uint16_t portNum,
    int nominalBufferSize)
{
    return pointer(new TcpServer(sf, io_context, portNum, nominalBufferSize));
}

bool TcpServer::start()
{
    if (halted.load(std::memory_order_acquire)) {
        if (!reConnect(std::chrono::milliseconds(1000))) {
            logger(0, "reconnect failed");
            acceptors.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            halted.store(false);
            initialConnect();
            if (halted) {
                if (!reConnect(std::chrono::milliseconds(1000))) {
                    logger(0, "reconnect part 2 failed");
                    return false;
                }
            }
        }
    }

    {  // scope for the lock_guard
        std::lock_guard<std::mutex> lock(accepting);
        if (!connections.empty()) {
            for (auto& conn : connections) {
                if (!conn->isReceiving()) {
                    conn->startReceive();
                }
            }
        }
    }
    bool success = true;
    for (auto& acc : acceptors) {
        if (!acc->start(
                TcpConnection::create(socket_factory, ioctx, bufferSize))) {
            logger(0, "acceptor has failed to start");
            success = false;
        }
    }
    return success;
}

void TcpServer::handle_accept(
    TcpAcceptor::pointer acc,
    TcpConnection::pointer new_connection)
{
    /*setting linger to 1 second*/
    new_connection->socket()->set_option_linger(true, 0);
    new_connection->socket()->set_option_no_delay(true);
    // Set options here
    if (halted.load()) {
        new_connection->close();
        return;
    }

    new_connection->setHandshakeModeServer();
    new_connection->handshake();

    new_connection->setDataCall(dataCall);
    new_connection->setErrorCall(errorCall);
    if (logFunction) {
        new_connection->setLoggingFunction(logFunction);
    }
    new_connection->startReceive();
    {  // scope for the lock_guard

        std::unique_lock<std::mutex> lock(accepting);
        if (!halted.load()) {
            connections.push_back(std::move(new_connection));
        } else {
            lock.unlock();
            new_connection->close();
            return;
        }
    }
    acc->start(TcpConnection::create(socket_factory, ioctx, bufferSize));
}

TcpConnection::pointer TcpServer::findSocket(int connectorID) const
{
    std::unique_lock<std::mutex> lock(accepting);
    auto ptr = std::find_if(
        connections.begin(),
        connections.end(),
        [connectorID](const auto& conn) {
            return (conn->getIdentifier() == connectorID);
        });
    if (ptr != connections.end()) {
        return *ptr;
    }
    return nullptr;
}

void TcpServer::close()
{
    halted = true;
    if (acceptors.size() == 1) {
        acceptors[0]->close();
    } else if (!acceptors.empty()) {
        // cancel first to give the threads some time to process
        for (auto& acc : acceptors) {
            acc->cancel();
        }
        for (auto& acc : acceptors) {
            acc->close();
        }
        acceptors.clear();
    }

    std::unique_lock<std::mutex> lock(accepting);
    auto sz = connections.size();
    lock.unlock();
    if (sz > 0) {
        for (decltype(sz) ii = 0; ii < sz; ++ii) {
            connections[ii]->closeNoWait();
        }
        for (decltype(sz) ii = 0; ii < sz; ++ii) {
            connections[ii]->waitOnClose();
        }
        connections.clear();
    }
}

void TcpServer::logger(int logLevel, const std::string& message)
{
    if (logFunction) {
        logFunction(logLevel, message);
    } else {
        if (logLevel == 0) {
            std::cerr << message << std::endl;
        } else {
            std::cout << message << '\n';
        }
    }
}

void TcpServer::setLoggingFunction(
    std::function<void(int loglevel, const std::string& logMessage)> logFunc)
{
    logFunction = std::move(logFunc);
}

}  // namespace gmlc::networking
