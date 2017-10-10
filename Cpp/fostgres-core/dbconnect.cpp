/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/db.hpp>

#include <mutex>


namespace {


    std::mutex g_cb_mut;
    std::vector<fostgres::cnx_callback_fn> g_callbacks;


}


fostlib::pg::connection fostgres::connection(
    const fostlib::json &config,
    const fostlib::nullable<fostlib::string> &zoneinfo,
    const fostlib::nullable<fostlib::string> &/*subrole*/
) {
    fostlib::pg::connection cnx(config);
    if ( zoneinfo ) {
        cnx.zoneinfo(zoneinfo.value());
    }
    std::unique_lock<std::mutex> lock{g_cb_mut};
    for ( auto &cb : g_callbacks )
        cb(cnx);
    return cnx;
}


void fostgres::register_connection_callback(cnx_callback_fn cb)  {
    std::unique_lock<std::mutex> lock{g_cb_mut};
    g_callbacks.push_back(std::move(cb));
}
