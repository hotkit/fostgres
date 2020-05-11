/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>

#include <f5/threading/map.hpp>


/*
    fostgres::responder
*/


namespace {
    using responder_map =
            f5::tsmap<fostlib::string, fostgres::responder_function>;

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


std::pair<boost::shared_ptr<fostlib::mime>, int> fostgres::response(
        fostlib::pg::connection &cnx,
        fostlib::json const &config,
        match const &m,
        fostlib::http::server::request &req) {
    auto fname = fostlib::coerce<fostlib::nullable<f5::u8view>>(
            m.configuration["return"]);
    if (fname) {
        auto returner = g_responders().find(fname.value());
        if (returner) { return returner(cnx, config, m, req); }
    }
    return response_csj(cnx, config, m, req);
}
