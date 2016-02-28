/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/urlhandler>


namespace fostgres {


    /// Turn response data into an actual response
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response(fostlib::json data);
    /// Turn response data into an actual response
    std::pair<boost::shared_ptr<fostlib::mime>, int>  response(
        const std::pair<std::vector<fostlib::string>, std::vector<fostlib::json>>&);


}

