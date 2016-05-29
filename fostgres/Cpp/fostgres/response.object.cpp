/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/log>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {


    std::pair<boost::shared_ptr<fostlib::mime>, int>  get(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        auto data = fostgres::sql(
                cnx,
                fostlib::coerce<fostlib::string>(m.configuration["GET"]),
                m.arguments);
        fostlib::json result;
        auto row = data.second.begin();
        if ( row == data.second.end() ) {
            // TODO Return proper 404
            throw fostlib::exceptions::not_implemented(__FUNCTION__);
        }
        auto record = *row;
        for ( std::size_t index{0}; index < record.size(); ++index ) {
            fostlib::insert(result, data.first[index], record[index]);
        }
        if ( ++row != data.second.end() ) {
            // TODO Return proper error
            throw fostlib::exceptions::not_implemented(__FUNCTION__);
        }
        const bool pretty = fostlib::coerce<fostlib::nullable<bool>>(config["pretty"]).value(true);
        boost::shared_ptr<fostlib::mime> response(
                new fostlib::text_body(fostlib::json::unparse(result, pretty),
                    fostlib::mime::mime_headers(), L"application/json"));
        return std::make_pair(response, 200);
    }


    fostlib::json calc_keys(const fostgres::match &m, const fostlib::json &config) {
        fostlib::json keys;
        for ( std::size_t index{0}; index != config.size(); ++index ) {
            auto key = fostlib::coerce<fostlib::string>(config[index]);
            fostlib::insert(keys, key, m.arguments[index]);
        }
        return keys;
    }
    fostlib::json calc_values(const fostlib::json &body, const fostlib::json config) {
        fostlib::json values;
        for ( auto value : config ) {
            auto vname = value.get<fostlib::string>().value();
            if ( body.has_key(vname) ) {
                fostlib::insert(values, vname, body[vname]);
            }
        }
        return values;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  put(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json body{
            fostlib::json::parse(
                fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        fostlib::json keys(calc_keys(m, m.configuration["PUT"]["keys"]));
        fostlib::json values(calc_values(body, m.configuration["PUT"]["attributes"]));
        fostlib::string relation = fostlib::coerce<fostlib::string>(m.configuration["PUT"]["table"]);
        cnx.upsert(relation.c_str(), keys, values).commit();
        return get(cnx, config, m, req);
    }
    std::pair<boost::shared_ptr<fostlib::mime>, int>  patch(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json body{
            fostlib::json::parse(
                fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        fostlib::json keys(calc_keys(m, m.configuration["PATCH"]["keys"]));
        fostlib::json values(calc_values(body, m.configuration["PATCH"]["attributes"]));
        fostlib::string relation = fostlib::coerce<fostlib::string>(m.configuration["PATCH"]["table"]);
        cnx.update(relation.c_str(), keys, values).commit();
        return get(cnx, config, m, req);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  response_object(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::pg::connection cnx{config};
        if ( req.method() == "GET" or req.method() == "HEAD" ) {
            return get(cnx, config, m, req);
        } else if ( req.method() == "PUT" ) {
            return put(cnx, config, m, req);
        } else if ( req.method() == "PATCH" ) {
            return patch(cnx, config, m, req);
        } else {
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "Invalid HTTP method -- should return 405");
        }
    }


    const fostgres::responder c_csj("object", response_object);


}

