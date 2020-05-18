/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/datum.hpp>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>

#include <fost/log>


namespace {
    const fostlib::json c_file("file");
    constexpr f5::u8view redacted{"XXXXREDACTED"};

    /// ASCII based case insensitive comparison for equality which performs
    /// the same check as the header `map` does for the key names
    template<typename L, typename R>
    inline bool ieq(L &&l, R &&r) {
        fostlib::detail::ascii_iless less{};
        return not(less(l, r) || less(r, l));
    }

    fostlib::nullable<fostlib::json> proc_datum(
            const fostlib::json &jsource,
            const std::vector<fostlib::string> &arguments,
            const fostlib::json &row,
            const fostlib::http::server::request &req) {
        if (jsource.isarray()) {
            auto source = fostlib::coerce<fostlib::jcursor>(jsource);
            if (source.size()) {
                fostlib::jcursor subpath(++source.begin(), source.end());
                if (source[0] == "request") {
                    auto val = req[subpath];
                    if (subpath.size() >= 2 && subpath[0] == "headers"
                        && ieq("authorization",
                               fostlib::coerce<std::optional<fostlib::string>>(
                                       subpath[1])
                                       .value_or(fostlib::string{}))) {
                        return fostlib::json{redacted};
                        return val;
                    } else {
                        return val;
                    }
                } else if (source[0] == "body" && row.has_key(subpath)) {
                    return row[subpath];
                }
            }
            return fostlib::null;
        } else {
            auto n = fostlib::coerce<fostlib::nullable<std::size_t>>(
                    jsource.get<int64_t>());
            if (n) {
                if (n.value() > 0 && n.value() <= arguments.size()) {
                    return fostlib::json(arguments[n.value() - 1]);
                }
            } else {
                auto s =
                        fostlib::coerce<fostlib::nullable<f5::u8view>>(jsource);
                if (s && row.has_key(s.value())) { return row[s.value()]; }
            }
        }
        return fostlib::null;
    }

    fostlib::nullable<fostlib::json> call_datum(
            const fostlib::string &name,
            const fostlib::json &defn,
            const std::vector<fostlib::string> &arguments,
            const fostlib::json &row,
            const fostlib::http::server::request &req) {
        auto logger = fostlib::log::debug(fostgres::c_fostgres);
        logger("", "Datum lookup")("in", "name", name)("in", "defn", defn)(
                "in", "row", row);
        if (defn["type"] == c_file) {
            return fostgres::file_upload(name, defn, row);
        } else if (defn["source"].isnull()) {
            if (row.has_key(name)) {
                logger("found", "name", name);
                logger("found", "value", row[name]);
                return row[name];
            }
            logger("not-found", name);
        } else {
            return proc_datum(defn["source"], arguments, row, req);
        }
        return fostlib::null;
    }
}


fostlib::nullable<fostlib::json> fostgres::datum(
        const fostlib::json &jsource,
        const std::vector<fostlib::string> &arguments,
        const fostlib::json &row,
        const fostlib::http::server::request &req) {
    auto value = proc_datum(jsource, arguments, row, req);
    if (not value) {
        return value;
    } else {
        auto const str =
                fostlib::coerce<std::optional<f5::u8view>>(value.value());
        return fostlib::coerce<std::optional<fostlib::json>>(
                fostlib::trim(str));
    }
}


fostlib::nullable<fostlib::json> fostgres::datum(
        const fostlib::string &name,
        const fostlib::json &defn,
        const std::vector<fostlib::string> &arguments,
        const fostlib::json &row,
        const fostlib::http::server::request &req) {
    auto value = call_datum(name, defn, arguments, row, req);
    if (not value) return value;
    auto const str = fostlib::coerce<std::optional<f5::u8view>>(value.value());
    if (str && defn["trim"] != fostlib::json(false)) {
        auto result = fostlib::coerce<std::optional<fostlib::json>>(
                fostlib::trim(str));
        return result;
    } else {
        return value;
    }
}
