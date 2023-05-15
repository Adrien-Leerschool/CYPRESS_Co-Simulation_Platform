/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
#include <asio/ssl.hpp>
#endif

#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include <iostream>

namespace gmlc::networking {
// abstract Socket class defining what functions are needed for the rest of the
// gmlc::networking library
class Socket : std::enable_shared_from_this<Socket> {
  public:
    /** blocking function call to write data to the socket
     *
     * @param data buffer of data to write
     * @param len size of the data to write
     */
    virtual std::size_t write_some(const void* data, std::size_t len) = 0;

    /** blocking function call to read data from the socket
     *
     * @param data buffer to fill with read data
     * @param len size of the buffer
     */
    virtual std::size_t read_some(void* data, std::size_t len) = 0;

    /** asynchronous function call to read data from the socket
     *
     * @param data buffer of data to write
     * @param len size of the data to write
     * @param cb function to call when data is written, with error code and
     * amount of data written
     */
    virtual void async_write_some(
        const void* data,
        std::size_t len,
        std::function<void(const std::error_code&, std::size_t)> cb) = 0;

    /** asynchronous function call to read data from the socket
     *
     * @param data buffer to fill with read data
     * @param len size of the buffer
     * @param cb function to call when data is read, with error code and size of
     * data read
     */
    virtual void async_read_some(
        void* data,
        std::size_t len,
        std::function<void(const std::error_code&, std::size_t)> cb) = 0;

    /** asynchronous function call to read data from the socket
     *
     * @param data buffer to fill with read data
     * @param len size of the buffer
     * @param cb function to call when data is read, with error code and size of
     * data read
     */
    virtual void async_read_some(
        std::vector<char>& data,
        std::size_t len,
        std::function<void(const std::error_code&, std::size_t)> cb) = 0;

    /** asynchronous function call to establish a connection
     *
     * @param h host to connect to
     * @param s service to connect to (port number, or name of a common unix
     * service)
     * @param cb function called when the connection is established
     */
    virtual void async_connect(
        std::string h,
        std::string s,
        std::function<void(const std::error_code&)> cb) = 0;

    /** perform any handshake step needed to establish a connection*/
    virtual void handshake() = 0;

    // use the socket with an asio acceptor; refactor TcpAcceptor class to use a
    // wrapper around the asio acceptor would remove the dependency on Asio
    virtual void use_with_acceptor(
        asio::ip::tcp::acceptor&,
        std::function<void(const std::error_code&)>) = 0;

    /** checks if the socket is open or not
     *
     * @return true if the socket is open, otherwise false
     */
    virtual bool is_open() const = 0;

    /** shutdown the socket connection
     *
     * @param ec set to what error occurred, if any
     */
    virtual std::error_code shutdown(std::error_code& ec) = 0;
    /** close the socket connection (shutdown preferred)
     *
     * @param ec set to what error occurred, if any
     */
    virtual std::error_code close(std::error_code& ec) = 0;

    /** cancel outstanding synchronous operations (connect, send, receive)*/
    virtual void cancel() = 0;

    /** set the TCP_NODELAY option on the socket
     *
     * @param b true if TCP_NODELAY should be enabled, otherwise false
     * @throws std::system_error thrown on error
     */
    virtual void set_option_no_delay(bool b) = 0;

    /** set the TCP_NODELAY option on the socket
     *
     * @param b true if tcp no_delay should be enabled, otherwise false
     * @param ec set to what error occurred, if any
     */
    virtual std::error_code
        set_option_no_delay(bool b, std::error_code& ec) = 0;

    /** set the TCP linger option and timeout for pending data to be sent
     * see this stackoverflow post for advice on what values to use
     * https://stackoverflow.com/questions/3757289/when-is-tcp-option-so-linger-0-required
     *
     * @param b true if linger should be enabled, otherwise false
     * @param t time to linger on close for pending data to be sent
     * @throws std::system_error thrown on failure
     */
    virtual void set_option_linger(bool b, uint16_t t) = 0;

    /** set the TCP linger option and timeout for pending data to be sent
     * see this stackoverflow post for advice on what values to use
     * https://stackoverflow.com/questions/3757289/when-is-tcp-option-so-linger-0-required
     *
     * @param b true if linger should be enabled, otherwise false
     * @param t time to linger on close for pending data to be sent
     * @param ec set to what error occurred, if any
     */
    virtual std::error_code
        set_option_linger(bool b, uint16_t t, std::error_code& ec) = 0;

    /** sets whether the socket acts as a client or server during a handshake to
     * establish a connection
     *
     * @param b true if the socket should act as a server for performing
     * handshakes, otherwise false
     */
    void set_handshake_mode_server(bool b) { handshake_server_ = b; }

  protected:
    bool handshake_server_ = false;  // whether the socket should act as a
                                     // server during handshake step
};

// Socket class using ASIO behind the scenes
template<class T>
class AsioSocket final : public Socket {
  public:
    // constructor for unencrypted Asio socket that takes an asio::io_context
    AsioSocket(asio::io_context& io_context) :
        socket_(io_context), resolver_(io_context)
    {
    }
#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
    // constructor for encrypted Asio socket that takes an asio::io_context and
    // asio::ssl::context
    AsioSocket(asio::io_context& io_context, asio::ssl::context& ssl_context) :
        socket_(io_context, ssl_context), resolver_(io_context)
    {
    }
#endif

    // handle reads and writes by calling the corresponding asio function
    std::size_t write_some(const void* data, std::size_t len)
    {
        return socket_.write_some(asio::buffer(data, len));
    }
    std::size_t read_some(void* data, std::size_t len)
    {
        return socket_.read_some(asio::buffer(data, len));
    }

    void async_write_some(
        const void* data,
        std::size_t len,
        std::function<void(const std::error_code&, std::size_t)> cb)
    {
        socket_.async_write_some(asio::buffer(data, len), cb);
    }

    void async_read_some(
        void* data,
        std::size_t len,
        std::function<void(const std::error_code&, std::size_t)> cb)
    {
        socket_.async_read_some(asio::buffer(data, len), cb);
    }
    void async_read_some(
        std::vector<char>& data,
        std::size_t max_sz,
        std::function<void(const std::error_code&, std::size_t)> cb)
    {
        socket_.async_read_some(asio::buffer(data, max_sz), cb);
    }

    // resolve the host to connect to, then use the async_connect asio function
    void async_connect(
        std::string host,
        std::string service,
        std::function<void(const std::error_code&)> cb)
    {
        asio::ip::tcp::resolver::query query(
            asio::ip::tcp::v4(), host, service);
        auto endpoint_iterator = resolver_.resolve(query);

        socket_.lowest_layer().async_connect(*endpoint_iterator, cb);
    }

    /* // potential future API addition - change SSL certificate verification
settings #ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION void
set_verify_mode(asio::ssl::verify_mode m) { if
constexpr(std::is_base_of<asio::ssl::stream<asio::ip::tcp::socket>, T>::value) {
socket_.set_verify_mode(m);
     }
 }
 void set_verify_callback(std::function<bool(bool, asio::ssl::verify_context&)>
cb) { if constexpr(std::is_base_of<asio::ssl::stream<asio::ip::tcp::socket>,
T>::value) { socket_.set_verify_callback(cb);
     }
 }
 #endif
*/

    // Perform handshake step, if protocol requires it (SSL)
    void handshake()
    {
#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
        // make sure the socket is an SSL stream, otherwise these methods won't
        // exist
        if constexpr (std::is_base_of<
                          asio::ssl::stream<asio::ip::tcp::socket>,
                          T>::value) {
            socket_.set_verify_mode(asio::ssl::verify_peer);
            socket_.set_verify_callback(
                [](bool preverified, asio::ssl::verify_context& /*ctx*/) {
                    return preverified;
                });
            socket_.handshake(
                handshake_server_ ? asio::ssl::stream_base::server :
                                    asio::ssl::stream_base::client);
        }
#endif
    }

    // sets up an asio acceptor to use this socket
    void use_with_acceptor(
        asio::ip::tcp::acceptor& acc,
        std::function<void(const std::error_code&)> cb)
    {
        acc.async_accept(socket_.lowest_layer(), cb);
    }

    // check if the underlying asio socket connection is open
    bool is_open() const
    {
        return socket_.lowest_layer().is_open();
    }

    // call the underlying asio functions to shutdown a connected socket
    std::error_code shutdown(std::error_code& ec)
    {
        return socket_.lowest_layer().shutdown(
            asio::ip::tcp::socket::shutdown_both, ec);
    }

    // call the underlying asio functions to close a socket (shutdown is
    // preferred)
    std::error_code close(std::error_code& ec)
    {
        return socket_.lowest_layer().close(ec);
    }

    // cancel outstanding asynchronous operations (connect, send, receive)
    // immediately
    void cancel()
    {
        socket_.lowest_layer().cancel();
    }

    // set_option templated functions are the same as the definitions in asio
    template<typename SettableSocketOption>
    void set_option(const SettableSocketOption& opt)
    {
        socket_.lowest_layer().set_option(opt);
    }
    template<typename SettableSocketOption>
    std::error_code
        set_option(const SettableSocketOption& opt, std::error_code& ec)
    {
        return socket_.lowest_layer().set_option(opt, ec);
    }

    // exposing some specific socket options used in an asio agnostic way
    void set_option_no_delay(bool b)
    {
        set_option(asio::ip::tcp::no_delay(b));
    }
    std::error_code set_option_no_delay(bool b, std::error_code& ec)
    {
        return set_option(asio::ip::tcp::no_delay(b), ec);
    }
    void set_option_linger(bool b, uint16_t timeout)
    {
        set_option(asio::socket_base::linger(b, timeout));
    }
    std::error_code
        set_option_linger(bool b, uint16_t timeout, std::error_code& ec)
    {
        return set_option(asio::socket_base::linger(b, timeout), ec);
    }

  private:
    T socket_;
    asio::ip::tcp::resolver resolver_;
};
}  // namespace gmlc::networking
