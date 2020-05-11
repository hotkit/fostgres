/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/postgres>
#include <fost/urlhandler>


namespace fostgres {


    /// Take JSON representing the datum location and the possible
    /// places it could be. Return it if found, otherwise return null.
    fostlib::nullable<fostlib::json>
            datum(const fostlib::json &source,
                  const std::vector<fostlib::string> &arguments,
                  const fostlib::json &row,
                  const fostlib::http::server::request &req);
    /// Take a column definition and a JSON object representing a datum
    /// and produce the data element for forwarding into the database
    /// if it exists
    fostlib::nullable<fostlib::json>
            datum(const fostlib::string &name,
                  const fostlib::json &defn,
                  const std::vector<fostlib::string> &arguments,
                  const fostlib::json &row,
                  const fostlib::http::server::request &req);

    /// Process a file upload datum and return the file name
    fostlib::nullable<fostlib::json> file_upload(
            f5::u8view name,
            fostlib::json const &defn,
            fostlib::json const &row);


    struct match;


    /// Responder function
    using responder_function =
            std::function<std::pair<boost::shared_ptr<fostlib::mime>, int>(
                    fostlib::pg::connection &,
                    const fostlib::json &,
                    const match &,
                    fostlib::http::server::request &)>;


    /// Register a responder with a const instance of this class
    struct responder {
        responder(fostlib::string name, responder_function);
    };


    /// Turn response data into an actual response taking into account the
    /// accept header
    std::pair<boost::shared_ptr<fostlib::mime>, int> response(
            fostlib::pg::connection &cnx,
            fostlib::json const &config,
            match const &,
            fostlib::http::server::request &);
    /// Turn response data into an actual response taking into account the
    /// accept header
    std::pair<boost::shared_ptr<fostlib::mime>, int> response_csj(
            fostlib::pg::connection &cnx,
            fostlib::json const &config,
            match const &,
            fostlib::http::server::request &);

    /// Generate a response for a single JSON object from a row of data
    std::pair<boost::shared_ptr<fostlib::mime>, int> response_object(
            std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>
                    &&data,
            const fostlib::json &config);


}
