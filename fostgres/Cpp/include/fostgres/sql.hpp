/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/urlhandler>


namespace fostgres {


    /// Execute the command and return the column names and data
    std::pair<std::vector<fostlib::string>, std::vector<fostlib::json>> sql(
        const fostlib::string &host, const fostlib::string &database, const fostlib::string &cmd);


}

