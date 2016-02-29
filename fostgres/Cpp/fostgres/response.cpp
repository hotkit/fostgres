/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/push_back>
#include <fostgres/response.hpp>


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config, const fostlib::json &data
) {
    const bool pretty = fostlib::coerce<fostlib::nullable<bool>>(config["pretty"]).value(true);
    boost::shared_ptr<fostlib::mime> response(
            new fostlib::text_body(fostlib::json::unparse(data, pretty),
                fostlib::mime::mime_headers(), L"text/plain"));
    return std::make_pair(response, 501);
}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config,
    const std::pair<std::vector<fostlib::string>, std::vector<fostlib::json>> &data
) {
    fostlib::json result;
    fostlib::insert(result, "@context", "http://www.kirit.com/fostgres/context/rows");
    { // Start with the header row
        fostlib::json header;
        for ( const auto &heading : data.first ) {
            fostlib::push_back(header, heading);
        }
        fostlib::insert(result, "columns", header);
    }
    { // Continue with rows
        fostlib::json rows;
        for ( const auto &row : data.second ) {
            fostlib::push_back(rows, row);
        }
        fostlib::insert(result, "rows", rows);
    }
    return response(config, result);
}

