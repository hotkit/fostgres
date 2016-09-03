/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fostgres/fg/fg.hpp>
#include <fost/urlhandler>


namespace fg {


    /// Can be used to check the result of running a request against a URL
    class testserver {
        fostlib::string viewname;
        fostlib::setting<json> host_config, view_config;

    public:
        testserver(const fg::frame &, const fostlib::string &viewname);

        /// Return the result of the GET request
        std::pair<boost::shared_ptr<fostlib::mime>, int > get(
            frame &stack, const fostlib::string &path);
        /// Return the result of the PATCH request
        std::pair<boost::shared_ptr<fostlib::mime>, int > patch(
            frame &stack, const fostlib::string &path, const fostlib::json &data);
        /// Return the result of the PUT request
        std::pair<boost::shared_ptr<fostlib::mime>, int > put(
            frame &stack, const fostlib::string &path, const fostlib::json &data);
        /// Return the result of the POST request
        std::pair<boost::shared_ptr<fostlib::mime>, int > post(
            frame &stack, const fostlib::string &path, const fostlib::json &data);
        /// Return the result of the DELETE request
        std::pair<boost::shared_ptr<fostlib::mime>, int > del(
            frame &stack, const fostlib::string &path);
    };


}

