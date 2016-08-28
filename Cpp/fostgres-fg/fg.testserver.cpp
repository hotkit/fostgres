/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/mime.hpp>
#include <fostgres/fg/fg.testserver.hpp>
#include <fost/insert>


/*
    fg::testserver
*/


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


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::get(
    frame &stack, const fostlib::string &path
) {
    fostlib::http::server::request request("GET",
        fostlib::coerce<fostlib::url::filepath_string>(path));
    return fostlib::urlhandler::router(fostlib::host("localhost"), viewname, request);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::patch(
    frame &stack, const fostlib::string &path, const fostlib::json &data
) {
    std::unique_ptr<fostlib::binary_body> body(mime_from_argument(stack, data));;
    fostlib::http::server::request request("PATCH",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return fostlib::urlhandler::router(fostlib::host("localhost"), viewname, request);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::put(
    frame &stack, const fostlib::string &path, const fostlib::json &data
) {
    std::unique_ptr<fostlib::binary_body> body(mime_from_argument(stack, data));;
    fostlib::http::server::request request("PUT",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return fostlib::urlhandler::router(fostlib::host("localhost"), viewname, request);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::del(
    frame &stack, const fostlib::string &path
) {
    fostlib::http::server::request request("DELETE",
        fostlib::coerce<fostlib::url::filepath_string>(path));
    return fostlib::urlhandler::router(fostlib::host("localhost"), viewname, request);
}

