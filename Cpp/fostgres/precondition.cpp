/**
    Copyright 2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "precondition.hpp"


namespace {


    fostlib::json
            header(const fostlib::http::server::request &req,
                   fsigma::frame &stack,
                   fostlib::json::const_iterator pos,
                   fostlib::json::const_iterator end) {
        auto const name =
                stack.resolve_string(stack.argument("name", pos, end));
        if (req.headers().exists(name)) {
            return fostlib::json{req.headers()[name].value()};
        } else {
            return fostlib::json{};
        }
    }


}


fsigma::frame
        fostgres::preconditions(const fostlib::http::server::request &req) {
    fsigma::frame f{nullptr};

    f.native["header"] = [&req](fsigma::frame &stack,
                                fostlib::json::const_iterator pos,
                                fostlib::json::const_iterator end) {
        return header(req, stack, pos, end);
    };

    return f;
}
