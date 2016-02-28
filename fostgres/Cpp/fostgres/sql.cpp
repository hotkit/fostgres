/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log>
#include <fost/postgres>
#include <fostgres/fostgres.hpp>
#include <fostgres/sql.hpp>


std::pair<std::vector<fostlib::string>, std::vector<fostlib::json>> fostgres::sql(
    const fostlib::string &host, const fostlib::string &database, const fostlib::string &cmd
) {
    auto logger = fostlib::log::debug(c_fostgres);
    logger("", "Executing SQL command")
        ("host", host)
        ("database", database)
        ("command", cmd);

    /// Execute the SQL we've been given
    fostlib::pg::connection cnx(host, database);
    auto rs = cnx.exec(fostlib::coerce<fostlib::utf8_string>(cmd));

    /// Calculate the column headings
    std::vector<fostlib::string> columns;

    /// Now iterate the rows and add them
    std::vector<fostlib::json> rows;
    for ( const auto &row : rs ) {
        fostlib::json data;
        for ( std::size_t index{0}; index < row.size(); ++index ) {
            fostlib::push_back(data, row[index]);
        }
        rows.push_back(data);
    }

    /// Return data suitable for sending to the browser
    return std::make_pair(std::move(columns), std::move(rows));
}

