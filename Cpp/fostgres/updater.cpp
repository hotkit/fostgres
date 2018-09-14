/**
    Copyright 2016-2018, Felspar Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "updater.hpp"

#include <fost/insert>


fostgres::updater::updater(
    fostlib::json config, fostlib::json mconf, fostlib::pg::connection &cnx,
    const fostgres::match &m, fostlib::http::server::request &req
) : relation(fostlib::coerce<fostlib::string>(mconf["table"])),
    config(config), col_config(mconf["columns"]),
    cnx(cnx), m(m), req(req)
{
    if ( mconf.has_key("returning") ) {
        const fostlib::json &ret_cols = mconf["returning"];
        std::transform(ret_cols.begin(), ret_cols.end(), std::back_inserter(returning_cols),
            [](const auto &s) { return fostlib::coerce<fostlib::string>(s); });
    }
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::data(const fostlib::json &body) {
    fostlib::json keys, values;
    for ( const auto &col_def : col_config.object() ) {
        const auto &key = col_def.first;
        auto data = fostgres::datum(key, col_def.second, m.arguments, body, req);

        const bool allow_schema =
            fostlib::coerce<std::optional<bool>>(
                col_def.second["allow$schema"]).value_or(false);
        if ( allow_schema && data && data.value().has_key("$schema") ) {
//             throw fostlib::exceptions::not_implemented(
//                 __PRETTY_FUNCTION__);
        }

        if ( col_def.second["key"].get(false) ) {
            // Key column
            if ( data ) {
                fostlib::insert(keys, key, data.value());
            } else {
                throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__,
                    "Key column doesn't have a value", key);
            }
        } else if ( data ) {
            // Value column
            fostlib::insert(values, key, data.value());
        }
    }
    return std::make_pair(keys, values);
}


std::pair<
    std::pair<boost::shared_ptr<fostlib::mime>, int>,
    std::pair<fostlib::json, fostlib::json>
> fostgres::updater::upsert(
    std::pair<boost::shared_ptr<fostlib::mime>, int> (*get)(
        fostlib::pg::connection &cnx,
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ),
    const fostlib::json &body
) {
    auto d = data(body);
    if ( get && returning_cols.size() ) {
        auto rs = cnx.upsert(relation.c_str(), d.first, d.second, returning_cols);
        auto result = fostgres::column_names(std::move(rs));
        return {get(cnx, std::move(result), config, m, req), d};
    } else {
        cnx.upsert(relation.c_str(), d.first, d.second);
    }
    return {{nullptr, 0}, d};
}


std::pair<fostlib::json, fostlib::json> fostgres::updater::update(const fostlib::json &body) {
    auto d = data(body);
    cnx.update(relation.c_str(), d.first, d.second);
    return d;
}

