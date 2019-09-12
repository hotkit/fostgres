/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/db.hpp>

#include <mutex>


fostlib::pg::connection fostgres::connection(
        const fostlib::json &config,
        const fostlib::nullable<fostlib::string> &zoneinfo,
        const fostlib::nullable<fostlib::string> & /*subrole*/
) {
    fostlib::pg::connection cnx(config);
    if (zoneinfo) { cnx.zoneinfo(zoneinfo.value()); }
    return cnx;
}
