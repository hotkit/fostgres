/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/response.hpp>
#include <f5/threading/map.hpp>


namespace {
    using responder_map = f5::tsmap<fostlib::string, fostgres::responder_function>;

    responder_map &g_responders() {
        static responder_map rm;
        return rm;
    }
}


/*
    fostgres::responder
*/


fostgres::responder::responder(fostlib::string name, responder_function fn) {
    g_responders().insert_or_assign(std::move(name), fn);
}


/*
    fostgres::response
*/


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config,
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data
) {
    auto fname = config["return"].get<fostlib::string>();
    if ( not fname.isnull() ) {
        auto returner = g_responders().find(fname.value());
        if ( returner ) {
            return returner(config, std::move(data));
        }
    }
    return response_csj(config, std::move(data));
}

