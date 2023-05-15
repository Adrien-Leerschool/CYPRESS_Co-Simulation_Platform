/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "Socket.h"
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
#include <asio/ssl.hpp>
#endif

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace gmlc::networking {
class SocketFactory : std::enable_shared_from_this<SocketFactory> {
  public:
    /** default constructor for a SocketFactory */
    SocketFactory() = default;

    /** create a SocketFactory with options using the values in the given config
     * file
     *
     * @param conf the config file to load or JSON config string to parse
     * @param is_file whether to treat the conf parameter as a file path or JSON
     * string; default is true
     */
    SocketFactory(const std::string& conf, bool is_file = true)
    {
        if (is_file) {
            load_json_config_file(conf);
        } else {
            parse_json_config(conf);
        }
    }

    /** create a socket using the provided asio::io_context
     *
     * @param io_context an asio::io_context reference to use for the created
     * socket
     * @return std::shared_ptr<Socket> for the created socket
     * @throws std::system_error thrown on failure
     */
    std::shared_ptr<Socket> create_socket(asio::io_context& io_context) const
    {
        if (!encrypted) {
            return std::make_shared<AsioSocket<asio::ip::tcp::socket>>(
                io_context);
        } else {
#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
            // SSL_CTX in OpenSSL is reference counted; asio::ssl::context is a
            // wrapper around a SSL_CTX* SSL_CTX should not be changed after
            // using it to create any SSL objects more details at:
            // https://www.openssl.org/docs/manmaster/man3/SSL_CTX_new.html
            asio::ssl::context ssl_context(asio::ssl::context::tls);

            if (!password.empty()) {
                ssl_context.set_password_callback(
                    [pw = this->password](auto /*max_len*/, auto /*purpose*/) {
                        return pw;
                    });
            }
            if (use_default_verify_paths) {
                ssl_context.set_default_verify_paths();
            }
            if (!verify_file.empty()) {
                ssl_context.load_verify_file(verify_file);
            }
            if (!verify_path.empty()) {
                ssl_context.add_verify_path(verify_path);
            }
            if (!certificate_chain_file.empty()) {
                ssl_context.use_certificate_chain_file(certificate_chain_file);
            }
            if (!certificate_file.empty()) {
                ssl_context.use_certificate_file(
                    certificate_file, asio::ssl::context::pem);
            }
            if (!private_key_file.empty()) {
                ssl_context.use_private_key_file(
                    private_key_file, asio::ssl::context::pem);
            }
            if (!rsa_private_key_file.empty()) {
                ssl_context.use_rsa_private_key_file(
                    rsa_private_key_file, asio::ssl::context::pem);
            }
            if (!tmp_dh_file.empty()) {
                ssl_context.use_tmp_dh_file(tmp_dh_file);
            }

            return std::make_shared<
                AsioSocket<asio::ssl::stream<asio::ip::tcp::socket>>>(
                io_context, ssl_context);
#else
            throw std::runtime_error(
                "gmlc::networking library not compiled with encryption support");
#endif
        }
    }

    /** set if the socket created should act as a server for handshakes when
     * establishing a connection
     *
     * @param b true if the created socket should act as the server in
     * handshake, otherwise false
     */
    void set_handshake_server(bool b)
    {
        handshake_server = b;
    }

    /** get if the socket created should act as a server for handshakes when
     * establishing a connection
     *
     * @return true if the created socket should fill the role of the server in
     * handshakes, otherwise false
     */
    bool get_handshake_server()
    {
        return handshake_server;
    }

    /** set whether the sockets created by this factory should be encrypted or
     * not
     *
     * @param b true if the sockets should be encrypted, otherwise false
     */
    void set_encrypted(bool b)
    {
        encrypted = b;
    }

    /** load settings into the SocketFactory from a JSON config file
     *
     * @param file the JSON file to load settings from
     */
    void load_json_config_file(std::string file);

    /** load settings into the SocketFactory from a JSON string
     *
     * @param conf_str the JSON string to parse settings from
     */
    void parse_json_config(std::string conf_str);

  private:
    // other options that could be created:
    // - ssl protocol/mode to use (tls, sslv3, etc); though tls should probably
    // always be used
    // - verify depth for how far down a certificate chain to go
    // - verify mode for one sided verification, or looser requirements
    // - custom verify and password callback functions
    bool encrypted{false};
    bool handshake_server{false};  // whether or not the socket should act like
                                   // a server for SSL handshake purposes
#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
    bool use_default_verify_paths{
        false};  // use default directories for finding CA files
    std::string verify_file;  // CA file used in verification for SSL
    std::string verify_path;  // a directory with CA files to use for SSL
                              // verification; each file must contain a single
                              // certificate, named using the subject name's
                              // hash and an extension of ".0".
    std::string certificate_chain_file;  // certificate chain file for SSL
    std::string certificate_file;  // certificate file for SSL
    std::string private_key_file;  // private key file for SSL
    std::string rsa_private_key_file;  // RSA private key file for SSL
    std::string tmp_dh_file;  // temporary Diffie-Hellman paramenter file for
                              // SSL
    std::string password;  // used in the password response callback when
                           // loading SSL certificate/key files
#endif
};
}  // namespace gmlc::networking
