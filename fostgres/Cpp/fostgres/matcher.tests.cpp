/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/matcher.hpp>
#include <fost/push_back>
#include <fost/test>


FSL_TEST_SUITE(matcher);


FSL_TEST_FUNCTION(empty) {
    FSL_CHECK(fostgres::matcher(fostlib::json(), "").isnull());
}


FSL_TEST_FUNCTION(args_mismatch_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    auto m = fostgres::matcher(config, "");
    FSL_CHECK(m.isnull());
}


FSL_TEST_FUNCTION(args_match_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    auto m = fostgres::matcher(config, "first");
    FSL_CHECK(not m.isnull());
    FSL_CHECK_EQ(m.value().arguments.size(), 1u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
}


FSL_TEST_FUNCTION(args_match_2) {
    fostlib::json config;
    fostlib::push_back(config, "path", 2);
    fostlib::push_back(config, "path", 1);
    auto m = fostgres::matcher(config, "second/first/");
    FSL_CHECK(not m.isnull());
    FSL_CHECK_EQ(m.value().arguments.size(), 2u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
    FSL_CHECK_EQ(m.value().arguments[1], "second");
}


FSL_TEST_FUNCTION(args_with_fixed_strings_match_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    fostlib::push_back(config, "path", "/foo");
    fostlib::push_back(config, "path", 2);
    auto m = fostgres::matcher(config, "first/foo/second/");
    FSL_CHECK(not m.isnull());
    FSL_CHECK_EQ(m.value().arguments.size(), 2u);
    FSL_CHECK_EQ(m.value().arguments[0], "first");
    FSL_CHECK_EQ(m.value().arguments[1], "second");
}


FSL_TEST_FUNCTION(args_with_fixed_strings_mismatch_1) {
    fostlib::json config;
    fostlib::push_back(config, "path", 1);
    fostlib::push_back(config, "path", "/foo");
    fostlib::push_back(config, "path", 2);
    auto m = fostgres::matcher(config, "first/bar/second/");
    FSL_CHECK(m.isnull());
}
