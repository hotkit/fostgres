/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.testserver.hpp"
#include <fost/insert>


namespace {
    fg::json hconfig(const fostlib::string &view) {
        fg::json hosts;
        fostlib::insert(hosts, "", view);
        return hosts;
    }
}


fg::testserver::testserver(const fostlib::string &vn)
: viewname(vn), host_config("fg.testserver.cpp", fostlib::urlhandler::c_hosts, hconfig(viewname)) {
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::put(
    frame &stack, const fostlib::string &path, const fostlib::json &data
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
    fostlib::http::server::request request("PUT",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return fostlib::urlhandler::router(fostlib::host("localhost"), viewname, request);
}

