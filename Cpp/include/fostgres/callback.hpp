/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/postgres>


namespace fostgres {


    /// Register a callback to be called when a database connection
    /// is established.
    using cnx_callback_fn = std::function<void(fostlib::pg::connection&, const fostlib::http::server::request&)>;
    /// Create a const static instance of this class giving it the lambda
    /// you want executed on each database connect.
    class register_cnx_callback {
    public:
        register_cnx_callback(cnx_callback_fn);
    };


}
