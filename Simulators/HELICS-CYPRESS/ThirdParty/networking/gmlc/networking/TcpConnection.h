/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "GuardedTypes.hpp"
#include "Socket.h"
#include "SocketFactory.h"
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
namespace gmlc {
namespace networking {
    /** tcp socket generation for a receiving server*/
    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
      public:
        /** enumeration of the possible states of a connection*/
        enum class ConnectionStates {
            PRESTART = -1,
            WAITING = 0,
            OPERATING = 1,
            HALTED = 3,
            CLOSED = 4,
        };

        using pointer = std::shared_ptr<TcpConnection>;
        /** create a connection to the specified host+port
         *
         * @throws std::system_error thrown on failure
         */
        static pointer create(
            asio::io_context& io_context,
            const std::string& connection,
            const std::string& port,
            size_t bufferSize = 10192)
        {
            return create(
                SocketFactory(), io_context, connection, port, bufferSize);
        }
        /** create a connection to the specified host+port using the given
         * SocketFactory
         *
         * @throws std::system_error thrown on failure
         */
        static pointer create(
            const SocketFactory& sf,
            asio::io_context& io_context,
            const std::string& connection,
            const std::string& port,
            size_t bufferSize = 10192);
        /** create an RxConnection object using the specified context and
         * bufferSize
         *
         * @throws std::system_error thrown on failure
         */
        static pointer create(asio::io_context& io_context, size_t bufferSize)
        {
            return create(SocketFactory(), io_context, bufferSize);
        }
        /** create an RxConnection object using the specified context,
         * bufferSize, and SocketFactory
         *
         * @throws std::system_error thrown on failure
         */
        static pointer create(
            const SocketFactory& sf,
            asio::io_context& io_context,
            size_t bufferSize)
        {
            return pointer(new TcpConnection(sf, io_context, bufferSize));
        }
        /** get the underlying socket object*/
        auto socket() { return socket_; }
        /** start the receiving loop*/
        void startReceive();
        /** cancel ongoing socket operations*/
        void cancel() { socket_->cancel(); }
        /** close the socket*/
        void close();
        /** perform the close actions but don't wait for them to be processed*/
        void closeNoWait();
        /** wait on the closing actions*/
        void waitOnClose();
        /**check if the connection is receiving data*/
        bool isReceiving() const { return receivingHalt.isActive(); }
        /** set the callback for the data object
         *
         * @throws std::runtime_error thrown on failure
         */
        void setDataCall(
            std::function<size_t(TcpConnection::pointer, const char*, size_t)>
                dataFunc);
        /** set the callback for an error
         *
         * @throws std::runtime_error thrown on failure
         */
        void setErrorCall(
            std::function<bool(TcpConnection::pointer, const std::error_code&)>
                errorFunc);
        /** set a logging function
         *
         * @throws std::runtime_error thrown on failure
         */
        void setLoggingFunction(
            std::function<void(int loglevel, const std::string& logMessage)>
                logFunc);
        /** send raw data
    @throws std::system_error on failure*/
        size_t send(const void* buffer, size_t dataLength);
        /** send a string
    @throws std::system_error on failure*/
        size_t send(const std::string& dataString);

        /** do a blocking receive on the socket
    @throws std::system_error on failure
    @return the number of bytes received
    */
        size_t receive(void* buffer, size_t maxDataSize);
        /**perform an asynchronous send operation
    @param buffer the data to send
    @param dataLength the length of the data
    @param callback a callback function of the form void handler(
    const std::error_code& error, // Result of operation.
    std::size_t bytes_transferred           // Number of bytes received.
    );
    */
        template<typename Process>
        void send_async(const void* buffer, size_t dataLength, Process callback)
        {
            socket_->async_write_some(buffer, dataLength, callback);
        }
        /**perform an asynchronous receive operation
    @param buffer the data to send
    @param dataLength the length of the data
    @param callback a callback function of the form void handler(
    const std::error_code& error, // Result of operation.
    std::size_t bytes_transferred           // Number of bytes received.
    );
    */
        template<typename Process>
        void async_receive(void* buffer, size_t dataLength, Process callback)
        {
            socket_->async_read_some(buffer, dataLength, callback);
        }

        /**perform an asynchronous receive operation
   @param callback the callback function to execute when data has been received
   with signature void(TcpConnection::pointer, const char *buffer, size_t
   dataLength, const std::error_code &error)
    */
        void async_receive(std::function<void(
                               TcpConnection::pointer,
                               const char* buffer,
                               size_t dataLength,
                               const std::error_code& error)> callback)
        {
            socket_->async_read_some(
                data,
                data.size(),
                [connection = shared_from_this(),
                 callback = std::move(callback)](
                    const std::error_code& error, size_t bytes_transferred) {
                    connection->handle_read(bytes_transferred, error, callback);
                });
        }
        /** check if the socket has finished the connection process*/
        bool isConnected() const
        {
            return (connected.isActive()) &&
                (!connectionError.load(std::memory_order_acquire));
        }
        /** wait until the socket has finished the connection process
    @param timeOut the number of ms to wait for the connection process to finish
    (<0) for no limit
    @return true if connected, false if the timeout was reached
    */
        bool waitUntilConnected(std::chrono::milliseconds timeOut);
        /** get the id code for the socket*/
        int getIdentifier() const { return idcode; }

        /** set the handshake mode used by the underlying socket to server*/
        void setHandshakeModeServer()
        {
            socket_->set_handshake_mode_server(true);
        }
        /** calls the handshake function of the underlying socket*/
        void handshake() { socket_->handshake(); }

      private:
        /** constructors creating a socket*/
        TcpConnection(asio::io_context& io_context, size_t bufferSize) :
            TcpConnection(SocketFactory(), io_context, bufferSize)
        {
        }
        TcpConnection(
            const SocketFactory& sf,
            asio::io_context& io_context,
            size_t bufferSize) :
            socket_(sf.create_socket(io_context)),
            context_(io_context), data(bufferSize), idcode(idcounter++)
        {
        }

        /** function for handling the asynchronous return from a read request*/
        void
            handle_read(const std::error_code& error, size_t bytes_transferred);
        void handle_read(
            size_t message_size,
            const std::error_code& error,
            std::function<void(
                TcpConnection::pointer,
                const char*,
                size_t,
                const std::error_code& error)> callback)
        {
            callback(shared_from_this(), data.data(), message_size, error);
        }

        void logger(int level, const std::string& message);
        static std::atomic<int> idcounter;

        std::atomic<size_t> residBufferSize{0};
        std::shared_ptr<Socket> socket_;
        asio::io_context& context_;
        std::vector<char> data;
        std::atomic<bool> triggerhalt{false};
        const bool connecting{false};
        gmlc::concurrency::TriggerVariable receivingHalt;
        std::atomic<bool> connectionError{false};
        gmlc::concurrency::TriggerVariable connected;  //!< variable indicating
                                                       //!< connectivity
        std::function<size_t(TcpConnection::pointer, const char*, size_t)>
            dataCall;
        std::function<bool(TcpConnection::pointer, const std::error_code&)>
            errorCall;
        std::function<void(int level, const std::string& logMessage)>
            logFunction;
        std::atomic<ConnectionStates> state{ConnectionStates::PRESTART};
        const int idcode;
        void connect_handler(const std::error_code& error);
    };

}  // namespace networking
}  // namespace gmlc
