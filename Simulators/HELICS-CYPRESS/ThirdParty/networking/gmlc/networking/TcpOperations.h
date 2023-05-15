/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

/** @file
@details function in this file are common function used between the different
TCP comms */

#include "TcpHelperClasses.h"

#include <chrono>
#include <memory>
#include <string>

class AsioContextManager;
namespace asio {
class io_context;
}  // namespace asio

namespace gmlc::networking {

/** establish a connection to a server by as associated optional timeout
@param[in] io_context the context to establish the connection
@param[in] host the address of the connection to establish
@param[in] port the port number to connect to
@param[in] timeOut the time to wait for the connection to be established if set
of <=0 the connection will not wait for verification
@throws std::system_error thrown on failure
*/
TcpConnection::pointer establishConnection(
    asio::io_context& io_context,
    const std::string& host,
    const std::string& port,
    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0));

/** establish a connection to a server by as associated optional timeout
@param[in] sf the SocketFactory to use for creating the socket
@param[in] io_context the context to establish the connection
@param[in] host the address of the connection to establish
@param[in] port the port number to connect to
@param[in] timeOut the time to wait for the connection to be established if set
of <=0 the connection will not wait for verification
@throws std::system_error thrown on failure
*/
TcpConnection::pointer establishConnection(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& host,
    const std::string& port,
    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0));

/** establish a connection to a server by as associated optional timeout
@param[in] io_context the context to establish the connection
@param[in] address the address of the connection to establish usually network
address:port
@param[in] timeOut the time to wait for the connection to be established if set
of <=0 the connection will not wait for verification
@throws std::system_error thrown on failure
*/
TcpConnection::pointer establishConnection(
    asio::io_context& io_context,
    const std::string& address,
    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0));

/** establish a connection to a server by as associated optional timeout
@param[in] sf the SocketFactory to use for creating the socket
@param[in] io_context the context to establish the connection
@param[in] address the address of the connection to establish usually network
address:port
@param[in] timeOut the time to wait for the connection to be established if set
of <=0 the connection will not wait for verification
@throws std::system_error thrown on failure
*/
TcpConnection::pointer establishConnection(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& address,
    std::chrono::milliseconds timeOut = std::chrono::milliseconds(0));

}  // namespace gmlc::networking
