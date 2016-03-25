/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/push_back>
#include <fostgres/response.hpp>


namespace {


    const fostgres::responder c_csj("object", fostgres::response_object);


}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response_object(
    const fostlib::json &config,
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data
) {
    fostlib::json result;
    auto row = data.second.begin();
    if ( row == data.second.end() ) {
        // TODO Return proper 404
        throw fostlib::exceptions::not_implemented(__FUNCTION__);
    }
    auto record = *row;
    for ( std::size_t index{0}; index < record.size(); ++index ) {
        fostlib::insert(result, data.first[index], record[index]);
    }
    if ( ++row != data.second.end() ) {
        // TODO Return proper error
        throw fostlib::exceptions::not_implemented(__FUNCTION__);
    }
    const bool pretty = fostlib::coerce<fostlib::nullable<bool>>(config["pretty"]).value(true);
    boost::shared_ptr<fostlib::mime> response(
            new fostlib::text_body(fostlib::json::unparse(result, pretty),
                fostlib::mime::mime_headers(), L"application/json"));
    return std::make_pair(response, 200);
}

