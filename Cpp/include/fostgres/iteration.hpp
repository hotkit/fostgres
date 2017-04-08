/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/postgres>


namespace fostgres {

    /// Add in the column names to a recordset
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>
        column_names(fostlib::pg::recordset &&rs);


}

