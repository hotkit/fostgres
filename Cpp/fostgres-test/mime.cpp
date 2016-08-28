/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "contains.hpp"
#include "mime.hpp"
#include <fost/insert>


namespace {
    fg::json mime_to_json(const fostlib::mime &body) {
        std::vector<unsigned char> data;
        for ( const auto &part : body ) {
            data.insert(data.end(),
                reinterpret_cast<const char *>(part.first),
                reinterpret_cast<const char *>(part.second));
        }
        return fostlib::json::parse(
            fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::utf8_string>(data)));
    }
}


std::unique_ptr<fostlib::binary_body> fg::mime_from_argument(
    frame &stack, const json &data
) {
    std::unique_ptr<fostlib::binary_body> body;
    if ( data.isatom() || data.isarray() ) {
        auto filename = fostlib::coerce<boost::filesystem::path>(stack.resolve_string(data));
        auto filedata = fostlib::utf::load_file(filename);
        body.reset(new fostlib::binary_body(
            filedata.std_str().c_str(), filedata.std_str().c_str() + filedata.std_str().length()));
        body->headers().set("Content-Type", fostlib::urlhandler::mime_type(filename));
    } else {
        auto bodydata = fostlib::json::unparse(data, false);
        body.reset(new fostlib::binary_body(
            bodydata.std_str().c_str(), bodydata.std_str().c_str() + bodydata.std_str().length()));
        body->headers().set("Content-Type", "application/json");
    }
    return body;
}


void fg::assert_comparable(const fostlib::mime &actual, const fostlib::mime &expected) {
//     if ( actual.headers()["Content-Type"].value() != expected.headers()["Content-Type"].value() ) {
//         fostlib::exceptions::not_implemented error(__func__, "Content-Type mismatch");
//         fostlib::insert(error.data(), "actual", actual.headers()["Content-Type"].value());
//         fostlib::insert(error.data(), "expected", expected.headers()["Content-Type"].value());
//         throw error;
//     }
    if ( actual.headers()["Content-Type"].value() == "application/json" ) {
        auto actual_body = mime_to_json(actual);
        auto expected_body = mime_to_json(expected);
        auto contains = fg::contains(actual_body, expected_body);
        if ( not contains.isnull() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "The expected response is not a sub-set of the returned response", contains.value());
        }
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "MIME type", actual.headers()["Content-Type"].value());
    }
}

