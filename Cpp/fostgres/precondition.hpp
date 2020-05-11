/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/http>
#include <fost/http.server.hpp>
#include <fostgres/fsigma.hpp>
#include <fostgres/matcher.hpp>


namespace fostgres {


    struct precondition_context {
        fostlib::http::server::request &req;
        fostgres::match &m;
    };


    /// Returns the base frame with the available preconditions
    fsigma::frame preconditions(precondition_context);


}
