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
    auto stack = fostgres::preconditions(
            req, std::vector<fostlib::string>{}, fostlib::json{});

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

FSL_TEST_FUNCTION(match) {
    fostlib::mime::mime_headers header;
    fostlib::http::server::request req(
            "GET", "/", std::make_unique<fostlib::binary_body>(header));
    std::vector<fostlib::string> matched_args;
    matched_args.push_back("first-arg");
    matched_args.push_back("second-arg");
    auto stack = fostgres::preconditions(req, matched_args, fostlib::json{});

    /// Can retrieve arguments from matcher
    fostlib::json args;
    fostlib::push_back(args, 1);
    FSL_CHECK_EQ(
            fsigma::call(stack, "match", args.begin(), args.end()),
            fostlib::json{"first-arg"});
    fostlib::jcursor{0}.set(args, 2);
    FSL_CHECK_EQ(
            fsigma::call(stack, "match", args.begin(), args.end()),
            fostlib::json{"second-arg"});

    /// Index out of range should return null
    fostlib::jcursor{0}.set(args, 3);
    FSL_CHECK_EQ(
            fsigma::call(stack, "match", args.begin(), args.end()),
            fostlib::json{});
    fostlib::jcursor{0}.set(args, -5);
    FSL_CHECK_EQ(
            fsigma::call(stack, "match", args.begin(), args.end()),
            fostlib::json{});
}