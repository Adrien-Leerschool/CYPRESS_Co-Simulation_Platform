/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <string>
#include <utility>

namespace gmlc::networking {

/** define keys for particular interfaces*/
enum class InterfaceTypes : char {
    TCP = 0,  //!< using tcp ports for communication
    UDP = 1,  //!< using udp ports for communication
    IP = 2,  //!< using both types of ports (tcp/or udp) for communication
    IPC = 3,  //!< using ipc locations
    INPROC = 4,  //!< using inproc sockets for communications
};

/** generate a string with a full address based on an interface string and port
number
@details  how things get merged depend on what interface is used some use port
number some do not

@param networkInterface a string with an interface description i.e 127.0.0.1
@param portNumber the number of the port to use
@return a string with the merged address
*/
std::string
    makePortAddress(const std::string& networkInterface, int portNumber);

/** extract a port number and interface string from an address number
@details,  if there is no port number it default to -1 this is true if none was
listed or the interface doesn't use port numbers

@param address a string with an network location description i.e 127.0.0.1:34
@return a pair with a string and int with the interface name and port number
*/
std::pair<std::string, int> extractInterfaceAndPort(const std::string& address);

/** extract a port number string and interface string from an address number
@details,  if there is no port number it default to empty string this is true if
none was listed or the interface doesn't use port numbers

@param address a string with an network location description i.e 127.0.0.1:34
@return a pair with 2 strings with the interface name and port number
*/
std::pair<std::string, std::string>
    extractInterfaceAndPortString(const std::string& address);

/** strip any protocol strings from the interface and return a new string
for example tcp://127.0.0.1 -> 127.0.0.1*/
std::string stripProtocol(const std::string& networkAddress);
/** strip any protocol strings from the interface and return a new string*/
void removeProtocol(std::string& networkAddress);

/** add a protocol url to the interface and return a new string*/
std::string
    addProtocol(const std::string& networkAddress, InterfaceTypes interfaceT);

/** add a protocol url to the interface modifying the string in place*/
void insertProtocol(std::string& networkAddress, InterfaceTypes interfaceT);

/** check if a specified address is v6 or v4
@return true if the address is a v6 address
 */
bool isIpv6(const std::string& address);

}  // namespace gmlc::networking
