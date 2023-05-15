/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <string>
#include <vector>

namespace gmlc::networking {
/** define the network access*/
enum class InterfaceNetworks : char {
    LOCAL = 0,  //!< just open local ports
    IPV4 = 4,  //!< use external ipv4 ports
    IPV6 = 6,  //!< use external ipv6 ports
    ALL = 10,  //!< use all external ports
};

/** create a combined address list with choices in a rough order of priority
based on if they appear in both lists, followed by the high priority addresses,
and low priority addresses last

@param high addresses that should be considered before low addresses
@param low addresses that should be considered last
@return a vector of strings of ip addresses ordered in roughly the priority they
should be used
 */
std::vector<std::string> prioritizeExternalAddresses(
    std::vector<std::string> high,
    std::vector<std::string> low);

/** get the external ipv4 address of the current computer
 */
std::string getLocalExternalAddressV4();

/** get the external ipv4 Ethernet address of the current computer that best
 * matches the listed server*/
std::string getLocalExternalAddress(const std::string& server);

/** get the external ipv4 Ethernet address of the current computer that best
 * matches the listed server*/
std::string getLocalExternalAddressV4(const std::string& server);

/** get the external ipv4 address of the current computer
 */
std::string getLocalExternalAddressV6();

/** get the external ipv4 Ethernet address of the current computer that best
 * matches the listed server*/
std::string getLocalExternalAddressV6(const std::string& server);

/** generate an interface that matches a defined server or network specification
 */
std::string generateMatchingInterfaceAddress(
    const std::string& server,
    InterfaceNetworks network = InterfaceNetworks::LOCAL);

}  // namespace gmlc::networking
