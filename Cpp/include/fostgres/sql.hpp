/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/urlhandler>
#include <fost/postgres>


namespace fostgres {


    struct match;


    /// Return a database connection
    fostlib::pg::connection connection(
        fostlib::json config, const fostlib::http::server::request &req);

    /// Add in the column names to a recordset
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>
        column_names(fostlib::pg::recordset &&rs);

    /// Execute the command and return the column names and data
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> sql(
        const fostlib::json &dsn, const fostlib::http::server::request &req,
        const fostlib::string &cmd);
    /// Execute the command with arguments and return the column names and data
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> sql(
        const fostlib::json &dsn, const fostlib::http::server::request &req,
        const fostlib::string &cmd, const std::vector<fostlib::string> &args);

    /// Execute the command and return the column names and data
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> sql(
        fostlib::pg::connection &, const fostlib::string &cmd);
    /// Execute the command with arguments and return the column names and data
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> sql(
        fostlib::pg::connection &, const fostlib::string &cmd, const std::vector<fostlib::string> &args);
    /// Execute the command with arguments and return the column names and data
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> sql(
        fostlib::pg::connection &, const fostlib::string &cmd, const std::vector<fostlib::json> &args);

    /// Return the data associated with a SELECT configuration (e.g. a GET)
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> select_data(
        fostlib::pg::connection &, const fostlib::json &select,
        const fostgres::match &m, const fostlib::http::server::request &req);


}

