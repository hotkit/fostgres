/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/fg/fg.hpp>
#include <fost/insert>
#include <fost/ua/cache>
#include <fost/ua/cache.detail.hpp>
#include <odin/fg/native.hpp>


fg::frame::builtin fg::lib::ua_expect = [](
            fg::frame &stack,
            fg::json::const_iterator pos,
            fg::json::const_iterator end) {
) {
    auto http_method =  stack.resolve_string(stack.argument("http_method", pos, end));
    auto url =  fostlib::url{stack.resolve_string(stack.argument("url", pos, end))};
    auto response_status =  int{stack.resolve_string(stack.argument("response_status", pos, end))};
    if (response_status >= 400) {
        fostlib::exceptions::exception http_error;
        switch (response_status) {
            case 401:
                http_error = fostlib::ua::unauthorized{url};
                break;
            case 403:
                http_error = fostlib::ua::forbidden{url};
                break;
            case 404: [[fallthrough]];
            case 410:
                http_error = fostlib::ua::resource_not_found{url};
                break;
            default:
                http_error = fostlib::ua::http_error{url, response_status};
        fostlib::ua::expect(http_method, url, http_error)
    } else if (response_status < 300) {
        auto response_body =  int{stack.resolve_string(stack.argument("response_body", pos, end))};
        fostlib::ua::expect(http_method, url, response_body);
    } else {
        throw fostlib::exceptions::not_implemented{
                __PRETTY_FUNCTION__,
                "HTTP Status " + response_status + " not implemented"};        
    }
    return fostlib::json();
}
