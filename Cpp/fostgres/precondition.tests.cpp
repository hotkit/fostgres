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
            req, std::vector<fostlib::string>{});

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
    fostlib::http::server::request req{"GET", "/"};
    std::vector<fostlib::string> matched_args;
    matched_args.push_back("first-arg");
    matched_args.push_back("second-arg");
    auto stack = fostgres::preconditions(req, matched_args);

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

FSL_TEST_FUNCTION(eq) {
    fostlib::mime::mime_headers heads;
    heads.add("UserID", "test");
    fostlib::http::server::request req{
            "GET", "/", std::make_unique<fostlib::binary_body>(heads)};
    std::vector<fostlib::string> matched_args;
    matched_args.push_back("test");
    auto stack = fostgres::preconditions(req, matched_args);

    /// eq will return the evaluating value
    fostlib::json args;
    fostlib::push_back(args, 1);
    FSL_CHECK_EQ(
            fsigma::call(stack, "eq", args.begin(), args.end()),
            fostlib::json{1});

    fostlib::jcursor{0}.set(args, fostlib::json{"random string"});
    FSL_CHECK_EQ(
            fsigma::call(stack, "eq", args.begin(), args.end()),
            fostlib::json{"random string"});

    /// return evaluating value when values are 
    /// equals ["random string", "random string"]
    fostlib::push_back(args, fostlib::json{"random string"});
    FSL_CHECK_EQ(
            fsigma::call(stack, "eq", args.begin(), args.end()),
            fostlib::json{"random string"});

    /// return null when values are not equals
    /// ["random string", "random string", "other-string"]
    fostlib::push_back(args, fostlib::json{"other-string"});
    FSL_CHECK_EQ(
            fsigma::call(stack, "eq", args.begin(), args.end()),
            fostlib::json{});

    /// eq evaluate the arguments before compare
    /// [ ["match", 1], ["header", "Authorization"] ]
    fostlib::json ar;
    fostlib::push_back(ar, 0, "match");
    fostlib::push_back(ar, 0, 1);
    fostlib::push_back(ar, 1, "header");
    fostlib::push_back(ar, 1, "UserID");
    FSL_CHECK_EQ(
        fsigma::call(stack, "eq", ar.begin(), ar.end()),
        fostlib::json{"test"});
}