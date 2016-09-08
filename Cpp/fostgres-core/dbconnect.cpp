/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/db.hpp>


fostlib::pg::connection fostgres::connection(
    const fostlib::json &config,
    const fostlib::nullable<fostlib::string> &/*zoneinfo*/,
    const fostlib::nullable<fostlib::string> &/*subrole*/
) {
    fostlib::pg::connection cnx(config);
    return cnx;
}

