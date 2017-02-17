/*
    Copyright 2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/response.hpp>

#include <fost/crypto>


fostlib::nullable<fostlib::json> fostgres::file_upload(
    const fostlib::string &name, const fostlib::json &defn, const fostlib::json &row
) {
    fostlib::json representation;
    if ( defn["source"].isnull() && row.has_key(name) ) {
        fostlib::base64_string value{fostlib::coerce<fostlib::string>(row[name]).c_str()};
        auto data = fostlib::coerce<std::vector<unsigned char>>(value);
        fostlib::digester hasher{fostlib::sha256};
        hasher << data;
        auto filename = fostlib::coerce<fostlib::hex_string>(hasher.digest()).underlying().underlying();
        return fostlib::json(filename.c_str());
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "File upload where `source` is specified");
    }
    return fostlib::null;
}

