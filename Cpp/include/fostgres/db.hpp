/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/urlhandler>
#include <fost/postgres>


namespace fostgres {


    /// Return a database connection. If a zoneinfo name is given then the
    /// Postgres "time zone" is set to that name. If a subrole name is given
    /// then the role is switched to after the database connection has
    /// been made.
    fostlib::pg::connection connection(const fostlib::json &config,
        const fostlib::nullable<fostlib::string> &zoneinfo,
        const fostlib::nullable<fostlib::string> &subrole = fostlib::null);


}
