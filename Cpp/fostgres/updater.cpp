/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "updater.hpp"

#include <fost/insert>


fostgres::updater::updater(
    const fostlib::json &config, fostlib::pg::connection &cnx,
    const fostgres::match &m, fostlib::http::server::request &req
) : relation(fostlib::coerce<fostlib::string>(config["table"])),
    col_config(config["columns"]),
    cnx(cnx), m(m), req(req)
{
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::write(const fostlib::json &body) {
    fostlib::json keys, values;
    for ( auto col_def = col_config.begin(); col_def != col_config.end(); ++col_def ) {
        auto key = fostlib::coerce<fostlib::string>(col_def.key());
        auto data = fostgres::datum(key, *col_def, m.arguments, body, req);
        if ( (*col_def)["key"].get(false) ) {
            // Key column
            if ( not data.isnull() ) {
                fostlib::insert(keys, key, data.value());
            } else {
                throw fostlib::exceptions::not_implemented(__func__,
                    "Key column doesn't have a value", key);
            }
        } else if ( not data.isnull() ) {
            // Value column
            fostlib::insert(values, key, data.value());
        }
    }
    cnx.upsert(relation.c_str(), keys, values);
    return std::make_pair(keys, values);
}

