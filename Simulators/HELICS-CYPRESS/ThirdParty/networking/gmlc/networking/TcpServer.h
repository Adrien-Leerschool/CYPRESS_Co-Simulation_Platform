/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "SocketFactory.h"
#include "TcpAcceptor.h"
#include "TcpConnection.h"

#include "GuardedTypes.hpp"
#include "gmlc/concurrency/TriggerVariable.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/** @file
various helper classes and functions for handling TCP connections
*/
namespace gmlc::networking {

/** helper class for a server*/

class TcpServer : public std::enable_shared_from_this<TcpServer> {
  public:
    using pointer = std::shared_ptr<TcpServer>;

    static pointer create(
        asio::io_context& io_context,
        const std::string& address,
        const std::string& port,
        bool reuse_port = false,
        int nominalBufferSize = 10192);
    static pointer create(
        const SocketFactory& sf,
        asio::io_context& io_context,
        const std::string& address,
        const std::string& port,
        bool reuse_port = false,
        int nominalBufferSize = 10192);

    static pointer create(
        asio::io_context& io_context,
        const std::string& address,
        uint16_t portNum,
        bool reuse_port = false,
        int nominalBufferSize = 10192);
    static pointer create(
        const SocketFactory& sf,
        asio::io_context& io_context,
        const std::string& address,
        uint16_t portNum,
        bool reuse_port = false,
        int nominalBufferSize = 10192);

    static pointer create(
        asio::io_context& io_context,
        uint16_t portNum,
        int nominalBufferSize = 10192);
    static pointer create(
        const SocketFactory& sf,
        asio::io_context& io_context,
        uint16_t portNum,
        int nominalBufferSize = 10192);

  public:
    ~TcpServer();
    /**set the port reuse flag */
    void setPortReuse(bool reuse) { reuse_address = reuse; }
    /** start accepting new connections
@return true if the start up was successful*/
    bool start();
    /** close the server*/
    void close();
    /** check if the server is ready to start*/
    bool isReady() const { return !(halted.load()); }
    /** reConnect the server with the same address*/
    bool reConnect(std::chrono::milliseconds timeOut);
    /** set the data callback*/
    void setDataCall(
        std::function<size_t(TcpConnection::pointer, const char*, size_t)>
            dataFunc)
    {
        dataCall = std::move(dataFunc);
    }
    /** set the error path callback*/
    void setErrorCall(
        std::function<bool(TcpConnection::pointer, const std::error_code&)>
            errorFunc)
    {
        errorCall = std::move(errorFunc);
    }
    /** set a logging function */
    void setLoggingFunction(
        std::function<void(int loglevel, const std::string& logMessage)>
            logFunc);
    void handle_accept(
        TcpAcceptor::pointer acc,
        TcpConnection::pointer new_connection);
    /** get a socket by it identification code*/
    TcpConnection::pointer findSocket(int connectorID) const;

  private:
    TcpServer(
        asio::io_context& io_context,
        const std::string& address,
        uint16_t portNum,
        bool port_reuse,
        int nominalBufferSize);
    TcpServer(
        const SocketFactory& sf,
        asio::io_context& io_context,
        const std::string& address,
        uint16_t portNum,
        bool port_reuse,
        int nominalBufferSize);

    TcpServer(
        asio::io_context& io_context,
        const std::string& address,
        const std::string& port,
        bool port_reuse,
        int nominalBufferSize);
    TcpServer(
        const SocketFactory& sf,
        asio::io_context& io_context,
        const std::string& address,
        const std::string& port,
        bool port_reuse,
        int nominalBufferSize);

    TcpServer(
        asio::io_context& io_context,
        uint16_t portNum,
        int nominalBufferSize);
    TcpServer(
        const SocketFactory& sf,
        asio::io_context& io_context,
        uint16_t portNum,
        int nominalBufferSize);

    void initialConnect();
    void logger(int level, const std::string& message);

    asio::io_context& ioctx;
    SocketFactory socket_factory;
    mutable std::mutex accepting;
    std::vector<TcpAcceptor::pointer> acceptors;
    std::vector<asio::ip::tcp::endpoint> endpoints;
    size_t bufferSize;
    std::function<size_t(TcpConnection::pointer, const char*, size_t)> dataCall;
    std::function<bool(TcpConnection::pointer, const std::error_code& error)>
        errorCall;
    std::function<void(int level, const std::string& logMessage)> logFunction;
    std::atomic<bool> halted{false};
    bool reuse_address = false;
    // this data structure is protected by the accepting mutex
    std::vector<TcpConnection::pointer> connections;
};

}  // namespace gmlc::networking
