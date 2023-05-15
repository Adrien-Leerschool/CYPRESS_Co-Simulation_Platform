/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "SocketFactory.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace gmlc::networking {
void SocketFactory::load_json_config_file(std::string file)
{
    std::ifstream ifs(file);
    std::stringstream strbuf;
    strbuf << ifs.rdbuf();
    parse_json_config(strbuf.str());
}

void SocketFactory::parse_json_config(std::string conf_str)
{
    json j = json::parse(conf_str);

    // parse SocketFactory settings; defaults to existing setting if option is
    // not present in the JSON file
    encrypted = j.value("encrypted", encrypted);
#ifdef GMLC_NETWORKING_ENABLE_ENCRYPTION
    password = j.value("password", password);
    use_default_verify_paths =
        j.value("use_default_verify_paths", use_default_verify_paths);
    verify_file = j.value("verify_file", verify_file);
    verify_path = j.value("verify_path", verify_path);
    certificate_chain_file =
        j.value("certificate_chain_file", certificate_chain_file);
    certificate_file = j.value("certificate_file", certificate_file);
    private_key_file = j.value("private_key_file", private_key_file);
    rsa_private_key_file =
        j.value("rsa_private_key_file", rsa_private_key_file);
    tmp_dh_file = j.value("tmp_dh_file", tmp_dh_file);
#endif
}
}  // namespace gmlc::networking
