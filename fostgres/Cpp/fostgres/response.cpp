/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>

#include <f5/threading/map.hpp>
#include <fost/log>


fostlib::nullable<fostlib::json> fostgres::datum(
    const fostlib::string &name,
    const fostlib::json &defn,
    const std::vector<fostlib::string> &arguments,
    const fostlib::json &row
) {
    auto logger = fostlib::log::debug(fostgres::c_fostgres);
    logger("", "Datum lookup")
        ("in", "name", name)
        ("in", "defn", defn)
        ("in", "row", row);
    if ( defn["source"].isnull() ) {
        if ( row.has_key(name) ) {
            logger("found", name);
            logger("found", "value", row[name]);
            return row[name];
        }
        logger("not-found", name);
    } else {
        auto n = fostlib::coerce<fostlib::nullable<std::size_t>>(
            defn["source"].get<int64_t>());
        logger("searching", "source-int", n);
        if ( not n.isnull() ) {
            if ( n.value() > 0 && n.value() <= arguments.size() ) {
                logger("found", "source", n);
                logger("found", "value", fostlib::json(arguments[n.value() -1]));
                return fostlib::json(arguments[n.value() -1]);
            }
        } else {
            auto s = defn["source"].get<fostlib::string>();
            logger("searching", "source-str", s);
            if ( not s.isnull() && row.has_key(s.value()) ) {
                logger("found", "source", s);
                logger("found", "value", row[s.value()]);
                return row[s.value()];
            }
        }
    }
    return fostlib::null;
}


/*
    fostgres::responder
*/


namespace {
    using responder_map = f5::tsmap<fostlib::string, fostgres::responder_function>;

    responder_map &g_responders() {
        static responder_map rm;
        return rm;
    }
}


fostgres::responder::responder(fostlib::string name, responder_function fn) {
    g_responders().insert_or_assign(std::move(name), fn);
}


/*
    fostgres::response
*/


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config, const match &m, fostlib::http::server::request &req
) {
    auto fname = m.configuration["return"].get<fostlib::string>();
    if ( not fname.isnull() ) {
        auto returner = g_responders().find(fname.value());
        if ( returner ) {
            return returner(config, m, req);
        }
    }
    return response_csj(config, m, req);
}

