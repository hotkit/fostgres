/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
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
    if ( config.has_key("returning") ) {
        const fostlib::json &ret_cols = config["returning"];
        std::transform(ret_cols.begin(), ret_cols.end(), std::back_inserter(returning_cols),
            [](const auto &s) { return fostlib::coerce<fostlib::string>(s); });
    }
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::data(const fostlib::json &body) {
    fostlib::json keys, values;
    for ( auto col_def = col_config.begin(); col_def != col_config.end(); ++col_def ) {
        auto key = fostlib::coerce<fostlib::string>(col_def.key());
        auto data = fostgres::datum(key, *col_def, m.arguments, body, req);
        if ( (*col_def)["key"].get(false) ) {
            // Key column
            if ( data ) {
                fostlib::insert(keys, key, data.value());
            } else {
                throw fostlib::exceptions::not_implemented(__func__,
                    "Key column doesn't have a value", key);
            }
        } else if ( data ) {
            // Value column
            fostlib::insert(values, key, data.value());
        }
    }
    return std::make_pair(keys, values);
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::upsert(const fostlib::json &body) {
    auto d = data(body);
    cnx.upsert(relation.c_str(), d.first, d.second);
    return d;
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::update(const fostlib::json &body) {
    auto d = data(body);
    cnx.update(relation.c_str(), d.first, d.second);
    return d;
}

