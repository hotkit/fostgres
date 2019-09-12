/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fost/postgres>


namespace fostgres {


    fostlib::pg::connection connection(
            fostlib::json,
            const fostlib::nullable<fostlib::string> &,
            const fostlib::http::server::request &);


    /// Register a callback to be called when a database connection
    /// is established.
    using cnx_callback_fn = std::function<void(
            fostlib::pg::connection &, const fostlib::http::server::request &)>;
    /// Create a `const static` instance of this class giving it the lambda
    /// you want executed on each database connect.
    class register_cnx_callback {
        cnx_callback_fn cb;

      public:
        register_cnx_callback(cnx_callback_fn);
        ~register_cnx_callback();

      private:
        friend fostlib::pg::connection fostgres::connection(
                fostlib::json,
                const fostlib::nullable<fostlib::string> &,
                const fostlib::http::server::request &);
        void operator()(
                fostlib::pg::connection &c,
                const fostlib::http::server::request &r) const {
            cb(c, r);
        }
    };


}
