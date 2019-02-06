/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "precondition.hpp"
#include <fost/test>


FSL_TEST_SUITE(preconditions);


FSL_TEST_FUNCTION(builtins) {
    fostlib::http::server::request req("PUT", "/");
    auto stack = fostgres::preconditions(req);
}
