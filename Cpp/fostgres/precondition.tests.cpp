/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "precondition.hpp"
#include <fost/push_back>
#include <fost/test>


FSL_TEST_SUITE(preconditions);


FSL_TEST_FUNCTION(header) {
    fostlib::mime::mime_headers heads;
    heads.add("Content-Type", "application/json");
    fostlib::http::server::request req(
            "GET", "/", std::make_unique<fostlib::binary_body>(heads));
    auto stack = fostgres::preconditions(req);

    fostlib::json args;
    fostlib::push_back(args, "Content-Type");
    FSL_CHECK_EQ(
            fsigma::call(stack, "header", args.begin(), args.end()),
            fostlib::json{"application/json"});
    fostlib::jcursor{0}.set(args, fostlib::json{"Not-a-header"});
    FSL_CHECK_EQ(
            fsigma::call(stack, "header", args.begin(), args.end()),
            fostlib::json{});
}
