/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#include "interfaceOperations.hpp"

#include "addressOperations.hpp"
#include "gmlc/netif/NetIF.hpp"

#ifndef GMLC_NETWORKING_DISABLE_ASIO
#include "AsioContextManager.h"
#include <asio/ip/host_name.hpp>
#include <asio/ip/tcp.hpp>
#endif

#include <algorithm>

namespace gmlc::networking {
std::vector<std::string> prioritizeExternalAddresses(
    std::vector<std::string> high,
    std::vector<std::string> low)
{
    std::vector<std::string> result;

    // Top choice: addresses that both lists contain (resolver + OS)
    for (const auto& r_addr : low) {
        if (std::find(high.begin(), high.end(), r_addr) != high.end()) {
            result.push_back(r_addr);
        }
    }
    // Second choice: high-priority addresses found by the OS (likely link-local
    // addresses or loop-back)
    for (const auto& i_addr : high) {
        // add the address if it isn't already in the list
        if (std::find(result.begin(), result.end(), i_addr) == result.end()) {
            result.push_back(i_addr);
        }
    }
    // Last choice: low-priority addresses returned by the resolver (OS doesn't
    // know about them so may be invalid)
    for (const auto& r_addr : low) {
        // add the address if it isn't already in the list
        if (std::find(low.begin(), low.end(), r_addr) == low.end()) {
            result.push_back(r_addr);
        }
    }

    return result;
}

template<class InputIt1, class InputIt2>
static auto
    matchcount(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    int cnt = 0;
    while (first1 != last1 && first2 != last2 && *first1 == *first2) {
        ++first1, ++first2, ++cnt;
    }
    return cnt;
}

std::string getLocalExternalAddressV4()
{
    std::string resolved_address;
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    auto srv = AsioContextManager::getContextPointer();

    asio::ip::tcp::resolver resolver(srv->getBaseContext());
    asio::ip::tcp::resolver::query query(
        asio::ip::tcp::v4(), asio::ip::host_name(), "");
    std::error_code ec;
    asio::ip::tcp::resolver::iterator it = resolver.resolve(query, ec);

    if (!ec) {
        asio::ip::tcp::endpoint endpoint = *it;
        resolved_address = endpoint.address().to_string();
    }
#endif
    auto interface_addresses = gmlc::netif::getInterfaceAddressesV4();

    // Return the resolved address if no interface addresses were found
    if (interface_addresses.empty()) {
        if (resolved_address.empty()) {
            return "0.0.0.0";
        }
        return resolved_address;
    }

    // Use the resolved address if it matches one of the interface addresses
    for (const auto& addr : interface_addresses) {
        if (addr == resolved_address) {
            return resolved_address;
        }
    }

    // Pick an interface that isn't an IPv4 loopback address, 127.0.0.1/8
    // or an IPv4 link-local address, 169.254.0.0/16
    std::string link_local_addr;
    for (auto addr : interface_addresses) {
        if (addr.rfind("127.", 0) != 0) {
            if (addr.rfind("169.254.", 0) != 0) {
                return addr;
            }
            if (link_local_addr.empty()) {
                link_local_addr = addr;
            }
        }
    }

    // Return a link-local address since no alternatives were found
    if (!link_local_addr.empty()) {
        return link_local_addr;
    }

    // Very likely that any address returned at this point won't be a working
    // external address
    return resolved_address;
}

std::string getLocalExternalAddressV4(const std::string& server)
{
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    auto srv = gmlc::networking::AsioContextManager::getContextPointer();

    asio::ip::tcp::resolver resolver(srv->getBaseContext());

    asio::ip::tcp::resolver::query query_server(
        asio::ip::tcp::v4(), server, "");
    std::error_code ec;
    asio::ip::tcp::resolver::iterator it_server =
        resolver.resolve(query_server, ec);
    if (ec) {
        return getLocalExternalAddressV4();
    }
    asio::ip::tcp::endpoint servep = *it_server;

    asio::ip::tcp::resolver::iterator end;

    auto sstring = (it_server == end) ? server : servep.address().to_string();
#else
    std::string sstring = server;
#endif

    auto interface_addresses = gmlc::netif::getInterfaceAddressesV4();

    std::vector<std::string> resolved_addresses;
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    asio::ip::tcp::resolver::query query(
        asio::ip::tcp::v4(), asio::ip::host_name(), "");
    asio::ip::tcp::resolver::iterator it = resolver.resolve(query, ec);
    if (ec) {
        return getLocalExternalAddressV4();
    }
    // asio::ip::tcp::endpoint endpoint = *it;

    while (it != end) {
        asio::ip::tcp::endpoint ept = *it;
        resolved_addresses.push_back(ept.address().to_string());
        ++it;
    }
#endif
    auto candidate_addresses =
        prioritizeExternalAddresses(interface_addresses, resolved_addresses);

    int cnt = 0;
    std::string def = candidate_addresses[0];
    cnt = matchcount(sstring.begin(), sstring.end(), def.begin(), def.end());
    for (auto ndef : candidate_addresses) {
        auto mcnt = matchcount(
            sstring.begin(), sstring.end(), ndef.begin(), ndef.end());
        if ((mcnt > cnt) && (mcnt >= 7)) {
            def = ndef;
            cnt = mcnt;
        }
    }
    return def;
}

std::string getLocalExternalAddressV6()
{
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    auto srv = gmlc::networking::AsioContextManager::getContextPointer();

    asio::ip::tcp::resolver resolver(srv->getBaseContext());
    asio::ip::tcp::resolver::query query(
        asio::ip::tcp::v6(), asio::ip::host_name(), "");
    asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
    asio::ip::tcp::endpoint endpoint = *it;

    auto resolved_address = endpoint.address().to_string();
#else
    std::string resolved_address;
#endif
    auto interface_addresses = gmlc::netif::getInterfaceAddressesV6();

    // Return the resolved address if no interface addresses were found
    if (interface_addresses.empty()) {
        return resolved_address;
    }

    // Use the resolved address if it matches one of the interface addresses
    if (std::find(
            interface_addresses.begin(),
            interface_addresses.end(),
            resolved_address) != interface_addresses.end()) {
        return resolved_address;
    }

    // Pick an interface that isn't the IPv6 loopback address, ::1/128
    // or an IPv6 link-local address, fe80::/16
    std::string link_local_addr;
    for (auto addr : interface_addresses) {
        if (addr != "::1") {
            if (addr.rfind("fe80:", 0) != 0) {
                return addr;
            }
            if (link_local_addr.empty()) {
                link_local_addr = addr;
            }
        }
    }

    // No other choices, so return a link local address if one was found
    if (!link_local_addr.empty()) {
        return link_local_addr;
    }

    // Very likely that any address returned at this point won't be a working
    // external address
    return resolved_address;
}

std::string getLocalExternalAddressV6(const std::string& server)
{
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    auto srv = gmlc::networking::AsioContextManager::getContextPointer();

    asio::ip::tcp::resolver resolver(srv->getBaseContext());

    asio::ip::tcp::resolver::query query_server(
        asio::ip::tcp::v6(), server, "");
    asio::ip::tcp::resolver::iterator it_server =
        resolver.resolve(query_server);
    asio::ip::tcp::endpoint servep = *it_server;
    asio::ip::tcp::resolver::iterator end;

    auto sstring = (it_server == end) ? server : servep.address().to_string();
#else
    std::string sstring = server;
#endif
    auto interface_addresses = gmlc::netif::getInterfaceAddressesV6();
    std::vector<std::string> resolved_addresses;
#ifndef GMLC_NETWORKING_DISABLE_ASIO
    asio::ip::tcp::resolver::query query(
        asio::ip::tcp::v6(), asio::ip::host_name(), "");
    asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
    // asio::ip::tcp::endpoint endpoint = *it;

    while (it != end) {
        asio::ip::tcp::endpoint ept = *it;
        resolved_addresses.push_back(ept.address().to_string());
        ++it;
    }
#endif
    auto candidate_addresses =
        prioritizeExternalAddresses(interface_addresses, resolved_addresses);

    int cnt = 0;
    std::string def = candidate_addresses[0];
    cnt = matchcount(sstring.begin(), sstring.end(), def.begin(), def.end());
    for (auto ndef : candidate_addresses) {
        auto mcnt = matchcount(
            sstring.begin(), sstring.end(), ndef.begin(), ndef.end());
        if ((mcnt > cnt) && (mcnt >= 7)) {
            def = ndef;
            cnt = mcnt;
        }
    }
    return def;
}

std::string getLocalExternalAddress(const std::string& server)
{
    if (isIpv6(server)) {
        return getLocalExternalAddressV6(server);
    }
    return getLocalExternalAddressV4(server);
}

std::string generateMatchingInterfaceAddress(
    const std::string& server,
    InterfaceNetworks network)
{
    std::string newInterface;
    switch (network) {
        case InterfaceNetworks::LOCAL:
            if (server.empty()) {
                newInterface = "tcp://127.0.0.1";
            } else {
                newInterface = getLocalExternalAddress(server);
            }
            break;
        case InterfaceNetworks::IPV4:
            if (server.empty()) {
                newInterface = "tcp://*";
            } else {
                newInterface = getLocalExternalAddressV4(server);
            }
            break;
        case InterfaceNetworks::IPV6:
            if (server.empty()) {
                newInterface = "tcp://*";
            } else {
                newInterface = getLocalExternalAddressV6(server);
            }
            break;
        case InterfaceNetworks::ALL:
            if (server.empty()) {
                newInterface = "tcp://*";
            } else {
                newInterface = getLocalExternalAddress(server);
            }
            break;
    }
    return newInterface;
}
}  // namespace gmlc::networking
