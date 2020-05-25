/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>


namespace fostgres {


    /// Take JSON representing the datum location and the possible
    /// places it could be. Return it if found, otherwise return null.
    fostlib::nullable<fostlib::json>
            datum(const fostlib::json &source,
                  const std::vector<fostlib::string> &arguments,
                  const fostlib::json &row,
                  const fostlib::http::server::request &req);
    /// Take a column definition and a JSON object representing a datum
    /// and produce the data element for forwarding into the database
    /// if it exists
    fostlib::nullable<fostlib::json>
            datum(const fostlib::string &name,
                  const fostlib::json &defn,
                  const std::vector<fostlib::string> &arguments,
                  const fostlib::json &row,
                  const fostlib::http::server::request &req);


}
