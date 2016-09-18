/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>

#include <fost/log>


fostlib::nullable<fostlib::json> fostgres::datum(
    const fostlib::json &jsource,
    const std::vector<fostlib::string> &arguments,
    const fostlib::json &row,
    const fostlib::http::server::request &req
) {
    if ( jsource.isarray() ) {
        auto source = fostlib::coerce<fostlib::jcursor>(jsource);
        if ( source.size() ) {
            fostlib::jcursor subpath(++source.begin(), source.end());
            if ( source[0] == "request" ) {
                return req[subpath];
            } else if ( source[0] == "body" && row.has_key(subpath) ) {
                return row[subpath];
            }
        }
        return fostlib::null;
    } else {
        auto n = fostlib::coerce<fostlib::nullable<std::size_t>>(jsource.get<int64_t>());
        if ( not n.isnull() ) {
            if ( n.value() > 0 && n.value() <= arguments.size() ) {
                return fostlib::json(arguments[n.value() -1]);
            }
        } else {
            auto s = jsource.get<fostlib::string>();
            if ( not s.isnull() && row.has_key(s.value()) ) {
                return row[s.value()];
            }
        }
    }
    return fostlib::null;
}


fostlib::nullable<fostlib::json> fostgres::datum(
    const fostlib::string &name,
    const fostlib::json &defn,
    const std::vector<fostlib::string> &arguments,
    const fostlib::json &row,
    const fostlib::http::server::request &req
) {
    auto logger = fostlib::log::debug(fostgres::c_fostgres);
    logger("", "Datum lookup")
        ("in", "name", name)
        ("in", "defn", defn)
        ("in", "row", row);
    if ( defn["source"].isnull() ) {
        if ( row.has_key(name) ) {
            logger("found", "name", name);
            logger("found", "value", row[name]);
            return row[name];
        }
        logger("not-found", name);
    } else {
        return datum(defn["source"], arguments, row, req);
    }
    return fostlib::null;
}

