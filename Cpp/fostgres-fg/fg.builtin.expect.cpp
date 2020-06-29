/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include<string>  

#include <fostgres/fg/fg.hpp>
#include <fost/ua/cache>
#include <fost/ua/cache.detail.hpp>
#include <fost/ua/exceptions.hpp>


fg::frame::builtin fg::lib::expect = [](
            fg::frame &stack,
            fg::json::const_iterator pos,
            fg::json::const_iterator end) {
    auto http_method =  stack.resolve_string(stack.argument("http_method", pos, end));
    auto url =  fostlib::url{stack.resolve_string(stack.argument("url", pos, end))};
    auto response_status =  stack.resolve_int(stack.argument("response_status", pos, end));
    if (response_status >= 400) {
        switch (response_status) {
            case 401:
                fostlib::ua::expect(http_method, url, fostlib::ua::unauthorized{url});
                break;
            case 403:
                fostlib::ua::expect(http_method, url, fostlib::ua::forbidden{url});
                break;
            case 404: [[fallthrough]];
            case 410:
                fostlib::ua::expect(http_method, url, fostlib::ua::resource_not_found{url});
                break;
            default:
                fostlib::ua::expect(http_method, url, fostlib::ua::http_error{url, static_cast<int>(response_status)});
        }
    } else if (response_status < 300) {
        // auto response_body =  fostlib::json::parse(stack.resolve_string(stack.argument("response_body", pos, end)));
        auto response_body = stack.argument("response_body", pos, end);
        fostlib::ua::expect(http_method, url, response_body);
    } else {
        throw fostlib::exceptions::not_implemented{
                __PRETTY_FUNCTION__,
                "HTTP Status " + std::to_string(response_status) + " not implemented"};        
    }
    return fostlib::json();
};
