/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log>
#include <fostgres/db.hpp>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {
    template<typename RS>
    std::vector<fostlib::string> columns(const RS &rs) {
        std::vector<fostlib::string> cols;
        std::size_t number{0};
        for ( const auto &c : rs.columns() ) {
            cols.push_back(c.value_or("un-named." + std::to_string(++number)));
        }
        return cols;
    }
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::column_names(
    fostlib::pg::recordset && rs
) {
    return std::make_pair(columns(rs), std::move(rs));
}

