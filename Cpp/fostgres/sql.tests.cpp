/*
    Copyright 2018, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/sql.hpp>
#include <fost/insert>
#include <fost/push_back>
#include <fost/test>

#include <cstdlib>


FSL_TEST_SUITE(sql);


FSL_TEST_FUNCTION(normal_config) {
    fostlib::json c;
    fostlib::insert(c, "dbname", "t1");
    fostlib::insert(c, "user", "u1");
    fostlib::http::server::request req;
    const auto res = fostgres::connection_config(c, req);
    FSL_CHECK_EQ(c, res);
}


FSL_TEST_FUNCTION(env_config) {
    fostlib::json c;
    fostlib::insert(c, "dbname", "t1");
    fostlib::insert(c, "user", "u1");
    fostlib::push_back(c, "host", "env");
    fostlib::push_back(c, "host", "FGTESTHOST");
    fostlib::http::server::request req;
    setenv("FGTESTHOST", "testhost.example.com", true);
    const auto res = fostgres::connection_config(c, req);
    fostlib::jcursor("host").replace(c, "testhost.example.com");
    FSL_CHECK_EQ(c, res);
}


FSL_TEST_FUNCTION(env_missing_config) {
    fostlib::json c;
    fostlib::insert(c, "dbname", "t1");
    fostlib::insert(c, "user", "u1");
    fostlib::push_back(c, "host", "env");
    fostlib::push_back(c, "host", "FGTESTHOSTXX");
    fostlib::http::server::request req;
    const auto res = fostgres::connection_config(c, req);
    fostlib::jcursor("host").del_key(c);
    FSL_CHECK_EQ(c, res);
}
