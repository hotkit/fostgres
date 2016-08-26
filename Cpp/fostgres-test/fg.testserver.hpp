/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include "fg.hpp"
#include <fost/urlhandler>


namespace fg {


    /// Can be used to check the result of running a request against a URL
    class testserver {
        fostlib::setting<json> host_config;

    public:
        testserver(const fostlib::string &viewname);

        /// Return the result of the POST request
        std::pair<boost::shared_ptr<fostlib::mime>, int > put(
            frame &stack, const fostlib::string &path, const fostlib::json &data);
    };


}

