/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "TcpConnection.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

namespace gmlc::networking {
using asio::ip::tcp;
using namespace std::chrono_literals;  // NOLINT

std::atomic<int> TcpConnection::idcounter{10};

void TcpConnection::startReceive()
{
    if (triggerhalt) {
        receivingHalt.trigger();
        return;
    }
    if (state == ConnectionStates::PRESTART) {
        receivingHalt.activate();
        connected.activate();
        state = ConnectionStates::WAITING;
    }
    ConnectionStates exp = ConnectionStates::WAITING;
    if (state.compare_exchange_strong(exp, ConnectionStates::OPERATING)) {
        if (!receivingHalt.isActive()) {
            receivingHalt.activate();
        }
        if (!triggerhalt) {
            socket_->async_read_some(
                data.data() + residBufferSize,
                data.size() - residBufferSize,
                [ptr = shared_from_this()](
                    const std::error_code& err, size_t bytes) {
                    ptr->handle_read(err, bytes);
                });
            if (triggerhalt) {
                // cancel previous operation if triggerhalt is now active
                socket_->cancel();
                // receivingHalt.trigger();
            }
        } else {
            state = ConnectionStates::HALTED;
            receivingHalt.trigger();
        }
    } else if (exp != ConnectionStates::OPERATING) {
        /*either halted or closed*/
        receivingHalt.trigger();
    }
}

void TcpConnection::setDataCall(
    std::function<size_t(TcpConnection::pointer, const char*, size_t)> dataFunc)
{
    if (state.load() == ConnectionStates::PRESTART) {
        dataCall = std::move(dataFunc);
    } else {
        throw(std::runtime_error(
            "cannot set data callback after socket is started"));
    }
}
void TcpConnection::setErrorCall(
    std::function<bool(TcpConnection::pointer, const std::error_code&)>
        errorFunc)
{
    if (state.load() == ConnectionStates::PRESTART) {
        errorCall = std::move(errorFunc);
    } else {
        throw(std::runtime_error(
            "cannot set error callback after socket is started"));
    }
}

void TcpConnection::setLoggingFunction(
    std::function<void(int loglevel, const std::string& logMessage)> logFunc)
{
    if (state.load() == ConnectionStates::PRESTART) {
        logFunction = std::move(logFunc);
    } else {
        throw(std::runtime_error(
            "cannot set logging function after socket is started"));
    }
}

void TcpConnection::handle_read(
    const std::error_code& error,
    size_t bytes_transferred)
{
    if (triggerhalt.load(std::memory_order_acquire)) {
        state = ConnectionStates::HALTED;
        receivingHalt.trigger();
        return;
    }
    if (!error) {
        auto used = dataCall(
            shared_from_this(),
            data.data(),
            bytes_transferred + residBufferSize);
        if (used < (bytes_transferred + residBufferSize)) {
            if (used > 0) {
                std::copy(
                    data.data() + used,
                    data.data() + bytes_transferred + residBufferSize,
                    data.data());
            }
            residBufferSize = bytes_transferred + residBufferSize - used;
        } else {
            residBufferSize = 0;
            data.assign(data.size(), 0);
        }
        state = ConnectionStates::WAITING;
        startReceive();
    } else if (error == asio::error::operation_aborted) {
        state = ConnectionStates::HALTED;
        receivingHalt.trigger();
        return;
    } else {
        // there was an error
        if (bytes_transferred > 0) {
            auto used = dataCall(
                shared_from_this(),
                data.data(),
                bytes_transferred + residBufferSize);
            if (used < (bytes_transferred + residBufferSize)) {
                if (used > 0) {
                    std::copy(
                        data.data() + used,
                        data.data() + bytes_transferred + residBufferSize,
                        data.data());
                }
                residBufferSize = bytes_transferred + residBufferSize - used;
            } else {
                residBufferSize = 0;
            }
        }
        if (errorCall) {
            if (errorCall(shared_from_this(), error)) {
                state = ConnectionStates::WAITING;
                startReceive();
            } else {
                state = ConnectionStates::HALTED;
                receivingHalt.trigger();
            }
        } else if (error != asio::error::eof) {
            if (error != asio::error::connection_reset) {
                logger(0, std::string("receive error ") + error.message());
            }
            state = ConnectionStates::HALTED;
            receivingHalt.trigger();
        } else {
            state = ConnectionStates::HALTED;
            receivingHalt.trigger();
        }
    }
}

void TcpConnection::logger(int logLevel, const std::string& message)
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

// socket_.set_option_linger(true, 2, ec);
void TcpConnection::close()
{
    closeNoWait();
    waitOnClose();
}

void TcpConnection::closeNoWait()
{
    triggerhalt.store(true);
    switch (state.load()) {
        case ConnectionStates::PRESTART:
            if (receivingHalt.isActive()) {
                receivingHalt.trigger();
            }
            break;
        case ConnectionStates::HALTED:
        case ConnectionStates::CLOSED:
            receivingHalt.trigger();
            break;
        default:
            break;
    }

    std::error_code ec;
    if (socket_->is_open()) {
        socket_->shutdown(ec);
        if (ec) {
            if ((ec.value() != asio::error::not_connected) &&
                (ec.value() != asio::error::connection_reset)) {
                logger(
                    0,
                    std::string("error occurred sending shutdown::") +
                        ec.message() + " " + std::to_string(ec.value()));
            }
            ec.clear();
        }
        socket_->close(ec);
    } else {
        socket_->close(ec);
    }
}

/** wait on the closing actions*/
void TcpConnection::waitOnClose()
{
    if (triggerhalt.load(std::memory_order_acquire)) {
        if (connecting) {
            connected.waitActivation();
        }

        while (!receivingHalt.wait_for(std::chrono::milliseconds(200))) {
            std::stringstream str;
            str << "wait timeout " << static_cast<int>(state.load()) << " "
                << socket_->is_open() << " " << receivingHalt.isTriggered();
            logger(1, str.str());
            str.clear();

            str << "wait info " << context_.stopped() << " " << connecting;
            logger(1, str.str());
        }
    } else {
        close();
    }
    state.store(ConnectionStates::CLOSED);
}

TcpConnection::pointer TcpConnection::create(
    const SocketFactory& sf,
    asio::io_context& io_context,
    const std::string& connection,
    const std::string& port,
    size_t bufferSize)
{
    auto ptr = pointer(new TcpConnection(sf, io_context, bufferSize));

    ptr->socket_->async_connect(
        connection, port, [ptr](const std::error_code& error) {
            ptr->connect_handler(error);
        });
    return ptr;
}

// connect callback used by the client establishing a TCP connection
void TcpConnection::connect_handler(const std::error_code& error)
{
    if (!error) {
        socket_->handshake();
        connected.activate();
        socket_->set_option_no_delay(true);
    } else {
        std::stringstream str;

        str << "connection error " << error.message()
            << ": code =" << error.value();
        logger(0, str.str());
        connectionError = true;
        connected.activate();
    }
}
size_t TcpConnection::send(const void* buffer, size_t dataLength)
{
    if (!isConnected()) {
        if (!waitUntilConnected(300ms)) {
            logger(0, "connection timeout waiting again");
        }
        if (!waitUntilConnected(200ms)) {
            logger(0, "connection timeout twice, now returning");
            return 0;
        }
    }

    size_t sz{0};
    size_t sent_size{dataLength};
    size_t p{0};
    int count{0};
    while (count++ < 5 &&
           (sz = socket_->write_some(
                reinterpret_cast<const char*>(buffer) + p, sent_size)) !=
               sent_size) {
        sent_size -= sz;
        p += sz;
        //   std::cerr << "DEBUG partial buffer sent" << std::endl;
    }
    if (count >= 5) {
        logger(0, "TcpConnection send terminated");
        return 0;
    }
    return dataLength;

    //  assert(sz == dataLength);
    //  return sz;
}

size_t TcpConnection::send(const std::string& dataString)
{
    size_t sz;
    sz = send(&dataString[0], dataString.size());
    return sz;
}

size_t TcpConnection::receive(void* buffer, size_t maxDataSize)
{
    return socket_->read_some(buffer, maxDataSize);
}

bool TcpConnection::waitUntilConnected(std::chrono::milliseconds timeOut)
{
    if (isConnected()) {
        return true;
    }
    if (timeOut < 0ms) {
        connected.waitActivation();
        return isConnected();
    }
    connected.wait_forActivation(timeOut);
    return isConnected();
}

}  // namespace gmlc::networking
