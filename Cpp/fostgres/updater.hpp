/**
    Copyright 2016-2018, Felspar Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
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

        updater(fostlib::json method_config,
            fostlib::pg::connection &, const fostgres::match &,
            fostlib::http::server::request &);

        /// Returning data
        const std::vector<fostlib::string> returning() const {
            return returning_cols;
        }

        [[nodiscard]] std::pair<
            std::pair<boost::shared_ptr<fostlib::mime>, int>,
            std::pair<fostlib::json, fostlib::json>> upsert(
                std::pair<boost::shared_ptr<fostlib::mime>, int> (*)(
                    fostlib::pg::connection &cnx,
                    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data,
                    const fostlib::json &config, const fostgres::match &m,
                    fostlib::http::server::request &req
                ),
                const fostlib::json &data,
                std::optional<std::size_t > row = {});
        [[nodiscard]] auto upsert(std::size_t row, const fostlib::json &data) {
            return upsert(nullptr, data, row);
        }
        std::pair<fostlib::json, fostlib::json> update(const fostlib::json &data);

    private:
        fostlib::json config, col_config;
        std::vector<fostlib::string> returning_cols;

        fostlib::pg::connection &cnx;
        const fostgres::match &m;
        fostlib::http::server::request &req;

        /// Break apart the data into the key and value parts
        std::pair<fostlib::json, fostlib::json> data(const fostlib::json &data);
    };


    std::pair<boost::shared_ptr<fostlib::mime>, int>  schema_check(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req,
        const fostlib::json &schema_config, const fostlib::json &instance,
        fostlib::jcursor dpos);


}

