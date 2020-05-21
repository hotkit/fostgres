/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/insert>
#include <fost/push_back>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {


    std::pair<boost::shared_ptr<fostlib::mime>, int>
            get(fostlib::pg::connection &cnx,
                fostlib::json const &config,
                fostgres::match const &m,
                fostlib::http::server::request &req) {
        auto data = m.arguments.size() ? fostgres::sql(
                            cnx,
                            fostlib::coerce<fostlib::string>(
                                    m.configuration["GET"]),
                            m.arguments)
                                       : fostgres::sql(
                                               cnx,
                                               fostlib::coerce<fostlib::string>(
                                                       m.configuration["GET"]));
        fostlib::json result;
        fostlib::insert(
                result, "@context",
                "http://www.kirit.com/fostgres/context/rows");
        { // Start with the header row
            fostlib::json header;
            for (const auto &heading : data.first) {
                fostlib::push_back(header, heading);
            }
            fostlib::insert(result, "columns", header);
        }
        { // Continue with rows
            fostlib::json rows;
            for (auto const &record : data.second) {
                fostlib::json row;
                for (std::size_t index{0}; index < record.size(); ++index) {
                    fostlib::push_back(row, record[index]);
                }
                fostlib::push_back(rows, row);
            }
            fostlib::insert(result, "rows", rows);
        }
        bool const pretty =
                fostlib::coerce<fostlib::nullable<bool>>(config["pretty"])
                        .value_or(true);
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                fostlib::json::unparse(result, pretty),
                fostlib::mime::mime_headers(), "application/json"));
        return std::make_pair(response, 200);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int> response_json_csv(
            fostlib::pg::connection &cnx,
            fostlib::json const &config,
            fostgres::match const &m,
            fostlib::http::server::request &req) {
        return get(cnx, config, m, req);
    }


    const fostgres::responder c_csj("json-csv", response_json_csv);


}
