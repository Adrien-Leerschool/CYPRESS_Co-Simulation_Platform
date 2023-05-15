/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "TcpConnection.h"
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

/** tcp acceptor*/
class TcpAcceptor : public std::enable_shared_from_this<TcpAcceptor> {
  public:
    /// enumeration of available states in the acceptor
    enum class AcceptingStates {
        OPENED = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        HALTED = 3,
        CLOSED = 4,
    };
    using pointer = std::shared_ptr<TcpAcceptor>;
    /** create an RxConnection object using the specified context and
     * bufferSize*/
    static pointer
        create(asio::io_context& io_context, asio::ip::tcp::endpoint& ep)
    {
        return pointer(new TcpAcceptor(io_context, ep));
    }

    static pointer create(asio::io_context& io_context, uint16_t port)
    {
        return pointer(new TcpAcceptor(io_context, port));
    }
    /** destructor to make sure everything is closed without threading issues*/
    ~TcpAcceptor()
    {
        try {
            close();
        }
        catch (...) {
        }
    }

    /** connect the acceptor to the socket*/
    bool connect();
    /** connect the acceptor to the socket if disconnected and try up to
     * timeout*/
    bool connect(std::chrono::milliseconds timeOut);
    /** start the acceptor*/
    bool start(TcpConnection::pointer conn);
    /** cancel pending operations*/
    void cancel() { acceptor_.cancel(); }
    /** close the socket*/
    void close();
    /** check if the acceptor is current accepting new connections*/
    bool isAccepting() const { return accepting.isActive(); }
    /** check if the acceptor is ready to begin accepting*/
    bool isConnected() const
    {
        return (state.load() == AcceptingStates::CONNECTED);
    }
    /** set the callback for the data object*/
    void setAcceptCall(
        std::function<void(TcpAcceptor::pointer, TcpConnection::pointer)>
            accFunc)
    {
        acceptCall = std::move(accFunc);
    }

    /** set the error path callback*/
    void setErrorCall(
        std::function<bool(TcpAcceptor::pointer, const std::error_code&)>
            errorFunc)
    {
        errorCall = std::move(errorFunc);
    }
    /** set a logging function */
    void setLoggingFunction(
        std::function<void(int loglevel, const std::string& logMessage)>
            logFunc);
    /** set an option on the underlying acceptor*/
    template<class X>
    void set_option(const X& option)
    {
        acceptor_.set_option(option);
    }
    /** generate a string from the associated endpoint*/
    std::string to_string() const;

  private:
    TcpAcceptor(asio::io_context& io_context, asio::ip::tcp::endpoint& ep);
    TcpAcceptor(asio::io_context& io_context, uint16_t port);
    /** function for handling the asynchronous return from a read request*/
    void handle_accept(
        TcpAcceptor::pointer ptr,
        TcpConnection::pointer new_connection,
        const std::error_code& error);

    void logger(int level, const std::string& message);
    asio::ip::tcp::endpoint endpoint_;
    asio::ip::tcp::acceptor acceptor_;
    std::function<void(TcpAcceptor::pointer, TcpConnection::pointer)>
        acceptCall;
    std::function<bool(TcpAcceptor::pointer, const std::error_code&)> errorCall;
    std::function<void(int level, const std::string& logMessage)> logFunction;
    std::atomic<AcceptingStates> state{AcceptingStates::OPENED};
    gmlc::concurrency::TriggerVariable accepting;
};

}  // namespace gmlc::networking
