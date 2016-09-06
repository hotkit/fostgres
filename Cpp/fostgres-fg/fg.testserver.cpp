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


    fg::json hconfig() {
        fg::json hosts;
        fostlib::insert(hosts, "", "fg.test");
        return hosts;
    }


    fg::json vconfig(const fg::frame &stack, const fostlib::string &view) {
        fg::json views;
        fostlib::insert(views, "view", "fost.middleware.request");
        fostlib::insert(views, "configuration", "headers", "__pgdsn", stack.lookup("pg.dsn"));
        fostlib::insert(views, "configuration", "view", view);
        return views;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int > perform(
        fg::frame &stack, fostlib::http::server::request &req,
        const fostlib::string &path, int expected_status
    ) {
        fostlib::host host("localhost");
        auto headers = stack.symbols["testserver.headers"];
        for ( auto h(headers.begin()); h != headers.end(); ++h ) {
            req.headers().set(fostlib::coerce<fostlib::string>(h.key()),
                fostlib::coerce<fostlib::string>(*h));
        }
        try {
            return fostlib::urlhandler::router(host, "fg.test", req);
        } catch ( fostlib::exceptions::not_implemented & ) {
            if ( expected_status == 501 ) {
                return fostlib::urlhandler::response_501(fg::json(), path, req, host);
            } else {
                throw;
            }
        }
    }


}


fg::testserver::testserver(const fg::frame &stack, const fostlib::string &vn)
: viewname(vn),
    host_config("fg.testserver.cpp", fostlib::urlhandler::c_hosts, hconfig()),
    view_config("fg.testserver.cpp", "webserver", "views/fg.test", vconfig(stack, viewname))
{
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::get(
    frame &stack, const fostlib::string &path, int expected_status
) {
    fostlib::http::server::request request("GET",
        fostlib::coerce<fostlib::url::filepath_string>(path));
    return perform(stack, request, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::patch(
    frame &stack, const fostlib::string &path, const fostlib::json &data, int expected_status
) {
    std::unique_ptr<fostlib::binary_body> body(mime_from_argument(stack, data));;
    fostlib::http::server::request request("PATCH",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return perform(stack, request, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::put(
    frame &stack, const fostlib::string &path, const fostlib::json &data, int expected_status
) {
    std::unique_ptr<fostlib::binary_body> body(mime_from_argument(stack, data));;
    fostlib::http::server::request request("PUT",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return perform(stack, request, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::post(
    frame &stack, const fostlib::string &path, const fostlib::json &data, int expected_status
) {
    std::unique_ptr<fostlib::binary_body> body(mime_from_argument(stack, data));;
    fostlib::http::server::request request("POST",
        fostlib::coerce<fostlib::url::filepath_string>(path), std::move(body));
    return perform(stack, request, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int > fg::testserver::del(
    frame &stack, const fostlib::string &path, int expected_status
) {
    fostlib::http::server::request request("DELETE",
        fostlib::coerce<fostlib::url::filepath_string>(path));
    return perform(stack, request, path, expected_status);
}

