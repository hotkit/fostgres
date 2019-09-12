/**
    Copyright 2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "datum.cpp"
#include <fost/push_back>
#include <fost/test>


FSL_TEST_SUITE(datum);

namespace {
    fostlib::json source_body() {
        std::vector<fostlib::string> source;
        source.push_back("body");
        source.push_back("username");
        fostlib::json defn;
        fostlib::insert(defn, "source", source);
        return defn;
    }

    fostlib::json source_header() {
        std::vector<fostlib::string> source;
        source.push_back("request");
        source.push_back("headers");
        source.push_back("__username");
        fostlib::json defn;
        fostlib::insert(defn, "source", source);
        return defn;
    }

    fostlib::json set_trim(fostlib::json defn, bool config) {
        return fostlib::insert(defn, "trim", config);
    }
}

FSL_TEST_FUNCTION(default_true_trim_from_body) {
    fostlib::json body;
    fostlib::insert(body, "username", "\u2008\u0020text\uFEFF\u3000");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"Test"}, source_body(), {}, body,
                    fostlib::http::server::request("POST", "/")),
            fostlib::json("text"));
}

FSL_TEST_FUNCTION(default_true_trim_from_request) {
    fostlib::http::server::request req("POST", "/");
    req.headers().set("__username", "\u00a0\u2001text\u1680\t\n\r\f");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"Test"}, source_header(), {},
                    fostlib::json(), req),
            fostlib::json("text"));
}

FSL_TEST_FUNCTION(no_source_in_configuration) {
    fostlib::json body;
    fostlib::insert(body, "username", "text");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"username"},
                    fostlib::json(fostlib::json::object_t()), {}, body,
                    fostlib::http::server::request("POST", "/")),
            fostlib::json("text"));
}

FSL_TEST_FUNCTION(no_source_in_configuration_default_trim) {
    fostlib::json body;
    fostlib::insert(body, "username", "\u00a0\u2001text\u1680\t\n\r\f");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"username"},
                    fostlib::json(fostlib::json::object_t()), {}, body,
                    fostlib::http::server::request("POST", "/")),
            fostlib::json("text"));
}

FSL_TEST_FUNCTION(no_source_in_configuration_trim_false) {
    fostlib::json body, config;
    fostlib::insert(body, "username", "\u0020text");
    fostlib::insert(config, "trim", false);

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"username"}, config, {}, body,
                    fostlib::http::server::request("POST", "/")),
            fostlib::json("\u0020text"));
}

FSL_TEST_FUNCTION(trim_config_true) {
    fostlib::json body;
    fostlib::insert(body, "username", "\u180e\u200atext\u200b\u205f");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"Test"}, set_trim(source_body(), true), {},
                    body, fostlib::http::server::request("POST", "/")),
            fostlib::json("text"));
}

FSL_TEST_FUNCTION(trim_config_false) {
    fostlib::json body;
    fostlib::insert(body, "username", "\u2008\u0020text\uFEFF\u3000");

    FSL_CHECK_EQ(
            fostgres::datum(
                    fostlib::string{"Test"}, set_trim(source_body(), false), {},
                    body, fostlib::http::server::request("POST", "/")),
            fostlib::json("\u2008\u0020text\uFEFF\u3000"));
}