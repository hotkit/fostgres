/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fostgres/fsigma.hpp>


namespace fostgres {


    /// Returns the base frame with the available preconditions
    fsigma::frame preconditions(
            const fostlib::http::server::request &,
            const std::vector<fostlib::string> &);


}
