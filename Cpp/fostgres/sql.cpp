/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log>
#include <fostgres/db.hpp>
#include <fostgres/fostgres.hpp>
#include <fostgres/sql.hpp>


namespace {
    template<typename RS>
    std::vector<fostlib::string> columns(const RS &rs) {
        std::vector<fostlib::string> cols;
        std::size_t number{0};
        for ( const auto &c : rs.columns() ) {
            cols.push_back(c.value("un-named." + std::to_string(++number)));
        }
        return cols;
    }
}


fostlib::pg::connection fostgres::connection(
    fostlib::json config, const fostlib::http::server::request &req
) {
    auto do_lookup = [&config, &req](const auto loc) {
            auto cfgvalue = config[loc];
            if ( cfgvalue.isarray() ) {
                auto lookup = fostlib::coerce<fostlib::jcursor>(cfgvalue);
                if ( lookup.size() && lookup[0] == "request" ) {
                    auto lookedup = req[fostlib::jcursor(++lookup.begin(), lookup.end())];
                    if ( lookedup.isnull() ) {
                        loc.del_key(config);
                    } else {
                        loc.replace(config, lookedup.value());
                    }
                } else {
                    throw fostlib::exceptions::not_implemented(__func__,
                        "Can't look up this position for the connection detail",
                        lookup);
                }
            }
        };

    static const fostlib::jcursor dbnameloc("dbname");
    do_lookup(dbnameloc);
    static const fostlib::jcursor hostloc("host");
    do_lookup(hostloc);
    static const fostlib::jcursor userloc("user");
    do_lookup(userloc);

    static const fostlib::jcursor ziloc("headers", "__pgzoneinfo");
    auto zoneinfo = req[ziloc];
    auto cnx = fostgres::connection(config,
        fostlib::coerce<fostlib::nullable<fostlib::string>>(zoneinfo));

    static const fostlib::jcursor sourceloc("headers", "X-Real-IP");
    auto source_addr = req[sourceloc];
    if ( not source_addr.isnull() ) {
        cnx.set_session("fostgres.source_addr",
            fostlib::coerce<fostlib::string>(source_addr.value()));
    }

    return cnx;
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::column_names(
    fostlib::pg::recordset && rs
) {
    return std::make_pair(columns(rs), std::move(rs));
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
    fostlib::pg::connection &cnx, const fostlib::string &cmd
) {
    auto logger = fostlib::log::debug(c_fostgres);
    logger("", "Executing SQL command")
        ("dsn", cnx.configuration())
        ("command", cmd);

    /// Execute the SQL we've been given
    auto rs = cnx.exec(fostlib::coerce<fostlib::utf8_string>(cmd));

    /// Return data suitable for sending to the browser
    return std::make_pair(columns(rs), std::move(rs));
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
    const fostlib::json &dsn, const fostlib::http::server::request &req,
    const fostlib::string &cmd
) {
    auto logger = fostlib::log::debug(c_fostgres);
    logger("", "Executing SQL command")
        ("command", cmd);

    /// Execute the SQL we've been given
    fostlib::pg::connection cnx(connection(dsn, req));
    logger("dsn", cnx.configuration());
    auto rs = cnx.exec(fostlib::coerce<fostlib::utf8_string>(cmd));

    /// Return data suitable for sending to the browser
    return std::make_pair(columns(rs), std::move(rs));
}


namespace {
    const auto sql_impl = [](auto &cnx, const auto &cmd, const auto &args) {
            auto logger = fostlib::log::debug(fostgres::c_fostgres);
            logger("", "Executing SQL command")
                ("dsn", cnx.configuration())
                ("command", cmd)
                ("args", args);

            /// Execute the SQL we've been given
            auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(cmd));
            auto rs = sp.exec(args);

            return std::make_pair(columns(rs), std::move(rs));
        };
}
std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
    fostlib::pg::connection &cnx,
    const fostlib::string &cmd, const std::vector<fostlib::string> &args
) {
    return sql_impl(cnx, cmd, args);
}
std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
    fostlib::pg::connection &cnx,
    const fostlib::string &cmd, const std::vector<fostlib::json> &args
) {
    return sql_impl(cnx, cmd, args);
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
    const fostlib::json &dsn, const fostlib::http::server::request &req,
    const fostlib::string &cmd,
    const std::vector<fostlib::string> &args
) {
    auto logger = fostlib::log::debug(c_fostgres);
    logger("", "Executing SQL command")
        ("command", cmd)
        ("args", args);

    /// Execute the SQL we've been given
    fostlib::pg::connection cnx(connection(dsn, req));
    logger("dsn", cnx.configuration());
    auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(cmd));
    auto rs = sp.exec(args);

    return std::make_pair(columns(rs), std::move(rs));
}

