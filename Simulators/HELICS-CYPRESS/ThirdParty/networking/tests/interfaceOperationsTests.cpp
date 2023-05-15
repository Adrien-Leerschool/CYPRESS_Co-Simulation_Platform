/*
Copyright (c) 2017-2021,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gmlc/networking/addressOperations.hpp"
#include "gmlc/networking/interfaceOperations.hpp"

TEST_CASE("local_address", "[interfaceOps]")
{
    auto netw = gmlc::networking::getLocalExternalAddressV4();
    CHECK_FALSE(gmlc::networking::isIpv6(netw));
    CHECK_FALSE(netw.empty());
    auto netw2 = gmlc::networking::getLocalExternalAddressV4("www.google.com");
    CHECK_FALSE(gmlc::networking::isIpv6(netw2));
    CHECK_FALSE(netw2.empty());
}

TEST_CASE("local_address_ipv6", "[interfaceOps]")
{
    try {
        auto netw = gmlc::networking::getLocalExternalAddressV6();
        CHECK_FALSE(netw.empty());
        CHECK(gmlc::networking::isIpv6(netw));
        auto netw2 = gmlc::networking::getLocalExternalAddressV6("2001:db8::1");
        CHECK(gmlc::networking::isIpv6(netw2));
        CHECK_FALSE(netw2.empty());
    }
    catch (...) {
        INFO("ipv6 not supported");
    }
}
