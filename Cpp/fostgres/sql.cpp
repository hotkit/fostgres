/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/log>
#include <fostgres/callback.hpp>
#include <fostgres/datum.hpp>
#include <fostgres/db.hpp>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/sql.hpp>

#include <mutex>


namespace {
    std::mutex g_cb_mut;
    auto &g_callbacks() {
        static std::set<fostgres::register_cnx_callback *> cbs;
        return cbs;
    }
}

fostgres::register_cnx_callback::register_cnx_callback(cnx_callback_fn cb)
: cb(std::move(cb)) {
    std::unique_lock<std::mutex> lock{g_cb_mut};
    g_callbacks().insert(this);
}
fostgres::register_cnx_callback::~register_cnx_callback() {
    std::unique_lock<std::mutex> lock{g_cb_mut};
    const auto pos = g_callbacks().find(this);
    if (pos != g_callbacks().end()) g_callbacks().erase(pos);
}


fostlib::json fostgres::connection_config(
        fostlib::json config, const fostlib::http::server::request &req) {
    /// If the configuration is empty or a JSON atom then it's not useful
    /// for fetching database settings out of, so throw it away and rely on
    /// the default lookups finding the right settings
    if (config.isnull() || config.isatom()) {
        config = fostlib::json::object_t();
    }

    auto do_lookup = [&config, &req](const auto &loc, const auto &fallback) {
        auto do_lookup = [&loc, &config, &req](const auto lookup) {
            if (lookup.size() && lookup[0] == "request") {
                auto lookedup =
                        req[fostlib::jcursor(++lookup.begin(), lookup.end())];
                if (not lookedup) {
                    if (config.has_key(loc)) loc.del_key(config);
                } else {
                    if (config.has_key(loc)) {
                        loc.replace(config, lookedup.value());
                    } else {
                        loc.insert(config, lookedup.value());
                    }
                }
            } else if (lookup.size() && lookup[0] == "env") {
                if (lookup.size() != 2) {
                    fostlib::log::warning(fostgres::c_fostgres)(
                            "",
                            "Environment lookup needs to have exactly one "
                            "item that is looked up")("config", "item", loc)(
                            "config", "current", config)("lookup", lookup);
                    loc.del_key(config);
                } else {
                    auto envname =
                            fostlib::coerce<fostlib::nullable<fostlib::string>>(
                                    lookup[1])
                                    .value();
                    const char *env = std::getenv(envname.shrink_to_fit());
                    if (env == nullptr)
                        loc.del_key(config);
                    else {
                        if (config.has_key(loc)) {
                            loc.replace(config, env);
                        } else {
                            loc.insert(config, env);
                        }
                    }
                }
            } else {
                fostlib::log::warning(fostgres::c_fostgres)(
                        "",
                        "Can't look up this position for the connection "
                        "detail")("allowed", 0, "request")("allowed", 1, "env")(
                        "config", "item",
                        loc)("config", "current", config)("lookup", lookup);
                loc.del_key(config);
            }
        };
        auto cfgvalue = config[loc];
        if (cfgvalue.isnull()) {
            do_lookup(fallback);
        } else if (cfgvalue.isarray()) {
            do_lookup(fostlib::coerce<fostlib::jcursor>(cfgvalue));
        }
    };

    static const fostlib::jcursor dbnameloc("dbname");
    static const fostlib::jcursor dbnamefallback(
            "request", "headers", "__pgdsn", "dbname");
    do_lookup(dbnameloc, dbnamefallback);
    static const fostlib::jcursor hostloc("host");
    static const fostlib::jcursor hostfallback(
            "request", "headers", "__pgdsn", "host");
    do_lookup(hostloc, hostfallback);
    static const fostlib::jcursor passwordloc("password");
    static const fostlib::jcursor passwordfallback(
            "request", "headers", "__pgdsn", "password");
    do_lookup(passwordloc, passwordfallback);
    static const fostlib::jcursor userloc("user");
    static const fostlib::jcursor userfallback(
            "request", "headers", "__pgdsn", "user");
    do_lookup(userloc, userfallback);

    return config;
}


fostlib::pg::connection fostgres::connection(
        fostlib::json config,
        const fostlib::nullable<fostlib::string> &zi,
        const fostlib::http::server::request &req) {
    auto cnx = fostgres::connection(config, zi);

    std::unique_lock<std::mutex> lock{g_cb_mut};
    for (const auto *const cb : g_callbacks()) (*cb)(cnx, req);

    cnx.set_session("fostgres.source_addr", req.remote_address().name());

    return cnx;
}


fostlib::pg::connection fostgres::connection(
        fostlib::json config, const fostlib::http::server::request &req) {
    config = connection_config(config, req);

    static const fostlib::jcursor ziloc("headers", "__pgzoneinfo");
    auto zoneinfo = req[ziloc];

    return connection(
            config,
            fostlib::coerce<fostlib::nullable<fostlib::string>>(zoneinfo), req);
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
        fostlib::pg::connection &cnx, const fostlib::string &cmd) {
    auto logger = fostlib::log::debug(c_fostgres);
    logger("",
           "Executing SQL command")("dsn", cnx.configuration())("command", cmd);

    /// Execute the SQL we've been given
    auto rs = cnx.exec(fostlib::coerce<fostlib::utf8_string>(cmd));

    /// Return data suitable for sending to the browser
    return column_names(std::move(rs));
}


namespace {
    const auto sql_impl = [](auto &cnx, const auto &cmd, const auto &args) {
        auto logger = fostlib::log::debug(fostgres::c_fostgres);
        logger("", "Executing SQL command")("dsn", cnx.configuration())(
                "command", cmd)("args", args);

        /// Execute the SQL we've been given
        auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(cmd));
        auto rs = sp.exec(args);

        return fostgres::column_names(std::move(rs));
    };
}
std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
        fostlib::pg::connection &cnx,
        const fostlib::string &cmd,
        const std::vector<fostlib::string> &args) {
    return sql_impl(cnx, cmd, args);
}
std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::sql(
        fostlib::pg::connection &cnx,
        const fostlib::string &cmd,
        const std::vector<fostlib::json> &args) {
    return sql_impl(cnx, cmd, args);
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>
        fostgres::select_data(
                fostlib::pg::connection &cnx,
                const fostlib::json &select,
                const fostgres::match &m,
                const fostlib::http::server::request &req) {
    if (select.isobject()) {
        if (select["arguments"].isnull()) {
            throw fostlib::exceptions::not_implemented(
                    __func__,
                    "SELECT configuration using an object must have a "
                    "'arguments' "
                    "key with the argument list in it",
                    select);
        }
        std::vector<fostlib::json> arguments;
        for (const auto &arg : select["arguments"]) {
            try {
                arguments.push_back(
                        fostgres::datum(arg, m.arguments, fostlib::json(), req)
                                .value_or(fostlib::json()));
            } catch (fostlib::exceptions::exception &e) {
                insert(e.data(), "datum", arg);
                throw;
            }
        }
        if (select["command"].isnull()) {
            throw fostlib::exceptions::not_implemented(
                    __func__,
                    "SELECT configuration using an object must have a "
                    "'command' "
                    "key with SQL in it",
                    select);
        }
        return fostgres::sql(
                cnx, fostlib::coerce<fostlib::string>(select["command"]),
                arguments);
    } else {
        return m.arguments.size()
                ? fostgres::sql(
                        cnx, fostlib::coerce<fostlib::string>(select),
                        m.arguments)
                : fostgres::sql(cnx, fostlib::coerce<fostlib::string>(select));
    }
}
