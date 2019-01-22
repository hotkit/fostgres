/**
    Copyright 2018-2019, Proteus Technologies Co Ltd.
   <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <fostgres/fostgres.hpp>
#include <f5/json/schema.loaders.hpp>
#include <fost/insert>
#include <fost/log>
#include <fost/postgres>


namespace {


    const fostlib::module c_fostgres_schema_loader(
            fostgres::c_fostgres, "fostgres-db-schema");


    f5::lstring c_select{"SELECT slug, schema FROM json_schema WHERE slug=$1"};


    const f5::json::schema_loader c_loader{
            "fostgres-db-schema",
            [](f5::u8view url,
               f5::json::value config) -> std::unique_ptr<f5::json::schema> {
                auto logger = fostlib::log::debug(c_fostgres_schema_loader);
                logger("requested-url", url);
                const auto prefix =
                        fostlib::coerce<f5::u8view>(config["prefix"]);
                if (url.starts_with(prefix)) {
                    logger("db", "config", config["dsn"]);
                    fostlib::pg::connection cnx{config["dsn"]};
                    auto sp = cnx.procedure(fostlib::utf8_string{c_select});
                    std::vector<fostlib::string> args;
                    args.push_back(url.substr(prefix.bytes()));
                    logger("db", "sql", fostlib::utf8_string{c_select});
                    logger("db", "args", args);
                    auto rs = sp.exec(args);
                    auto pos = rs.begin();
                    if (pos == rs.end()) {
                        logger("found", false);
                        logger("reason", "No row found in database");
                        return {};
                    }
                    auto schema = (*pos)[1u];
                    fostlib::insert(
                            schema, "$id",
                            fostlib::url{
                                    prefix
                                    + fostlib::coerce<f5::u8view>((*pos)[0u])});
                    logger("found", true);
                    return std::make_unique<f5::json::schema>(
                            fostlib::url{prefix}, schema);
                } else {
                    logger("prefix", prefix);
                    logger("reason", "URL Prefix didn't match");
                    logger("found", false);
                }
                return {};
            }};


}
