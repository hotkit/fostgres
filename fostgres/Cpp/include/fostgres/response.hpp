/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/postgres>
#include <fost/urlhandler>


namespace fostgres {


    /// Turn response data into an actual response
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response(
        const fostlib::json &config, const fostlib::json &data);

    /// Turn response data into an actual response taking into account the accept header
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response(
        const fostlib::json &config,
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>&&);


    /// Turn the response data into CSV like JSON no matter the accept header
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response_json_csv(
        const fostlib::json &config,
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>&&);
    /// Turn the response data into CSJ no matter the accept header
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response_csj(
        const fostlib::json &config,
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>&&);


}

