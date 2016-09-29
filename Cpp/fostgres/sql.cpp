/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
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
            cols.push_back(c.value("un-named." + std::to_string(++number)));
        }
        return cols;
    }
}


fostlib::pg::connection fostgres::connection(
    fostlib::json config, const fostlib::http::server::request &req
) {
    if ( config.isnull() ) {
        config = fostlib::json::object_t();
    }

    auto do_lookup = [&config, &req](const auto &loc, const auto &fallback) {
            auto do_lookup = [&loc, &config, &req](const auto lookup) {
                    if ( lookup.size() && lookup[0] == "request" ) {
                        auto lookedup = req[fostlib::jcursor(++lookup.begin(), lookup.end())];
                        if ( lookedup.isnull() ) {
                            if ( config.has_key(loc) ) loc.del_key(config);
                        } else {
                            if ( config.has_key(loc) ) {
                                loc.replace(config, lookedup.value());
                            } else {
                                loc.insert(config, lookedup.value());
                            }
                        }
                    } else {
                        throw fostlib::exceptions::not_implemented(__func__,
                            "Can't look up this position for the connection detail",
                            lookup);
                    }
                };
            auto cfgvalue = config[loc];
            if ( cfgvalue.isnull() ) {
                do_lookup(fallback);
            } else if ( cfgvalue.isarray() ) {
                do_lookup(fostlib::coerce<fostlib::jcursor>(cfgvalue));
            }
        };

    static const fostlib::jcursor dbnameloc("dbname");
    static const fostlib::jcursor dbnamefallback("request", "headers", "__pgdsn", "dbname");
    do_lookup(dbnameloc, dbnamefallback);
    static const fostlib::jcursor hostloc("host");
    static const fostlib::jcursor hostfallback("request", "headers", "__pgdsn", "host");
    do_lookup(hostloc, hostfallback);
    static const fostlib::jcursor userloc("user");
    static const fostlib::jcursor userfallback("request", "headers", "__pgdsn", "user");
    do_lookup(userloc, userfallback);

    static const fostlib::jcursor ziloc("headers", "__pgzoneinfo");
    auto zoneinfo = req[ziloc];
    auto cnx = fostgres::connection(config,
        fostlib::coerce<fostlib::nullable<fostlib::string>>(zoneinfo));

    cnx.set_session("fostgres.source_addr", req.remote_address().name());

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


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::select_data(
    fostlib::pg::connection &cnx, const fostlib::json &select,
    const fostgres::match &m, const fostlib::http::server::request &req
) {
    if ( select.isobject() ) {
        if ( select ["arguments"].isnull() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "SELECT configuration using an object must have a 'arguments' "
                "key with the argument list in it", select);
        }
        std::vector<fostlib::json> arguments;
        for ( const auto &arg : select["arguments"] ) {
            try {
                arguments.push_back(fostgres::datum(
                    arg, m.arguments, fostlib::json(), req).value(fostlib::json()));
            } catch ( fostlib::exceptions::exception &e ) {
                insert(e.data(), "datum", arg);
                throw;
            }
        }
        if ( select ["command"].isnull() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "SELECT configuration using an object must have a 'command' "
                "key with SQL in it", select);
        }
        return fostgres::sql(cnx, fostlib::coerce<fostlib::string>(select["command"]), arguments);
    } else {
        return m.arguments.size()
            ? fostgres::sql(cnx, fostlib::coerce<fostlib::string>(select), m.arguments)
            : fostgres::sql(cnx, fostlib::coerce<fostlib::string>(select));
    }
}

