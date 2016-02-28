/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/response.hpp>


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(fostlib::json data) {
    boost::shared_ptr<fostlib::mime> response(
            new fostlib::text_body(fostlib::json::unparse(data, true),
                fostlib::mime::mime_headers(), L"text/plain"));
    return std::make_pair(response, 501);
}

