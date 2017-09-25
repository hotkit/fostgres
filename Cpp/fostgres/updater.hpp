/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace fostgres {


    /// Internal class used for PATCH and PUT
    class updater {
    public:
        const fostlib::string relation;

        updater(const fostlib::json &config, fostlib::pg::connection &,
            const fostgres::match &, fostlib::http::server::request &);

        /// Break apart the data into the key and value parts
        std::pair<fostlib::json, fostlib::json> data(const fostlib::json &data);
        /// Returning data
        const std::vector<fostlib::string> returning() const {
            return returning_cols;
        }

        std::pair<fostlib::json, fostlib::json> upsert(const fostlib::json &data);
        std::pair<fostlib::json, fostlib::json> update(const fostlib::json &data);

    private:
        fostlib::json col_config;
        std::vector<fostlib::string> returning_cols;

        fostlib::pg::connection &cnx;
        const fostgres::match &m;
        fostlib::http::server::request &req;
    };


}

