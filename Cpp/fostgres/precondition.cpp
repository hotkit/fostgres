/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "precondition.hpp"
#include <fost/log>

namespace {


    fostlib::json
            header(const fostlib::http::server::request &req,
                   fsigma::frame &stack,
                   fostlib::json::const_iterator pos,
                   fostlib::json::const_iterator end) {
        auto const name =
                stack.resolve_string(stack.argument("name", pos, end));
        if (req.headers().exists(name)) {
            return fostlib::json{req.headers()[name].value()};
        } else {
            return fostlib::json{};
        }
    }
    fostlib::json
            eq(fsigma::frame &stack,
               fostlib::json::const_iterator pos,
               fostlib::json::const_iterator end) {
        auto const val = stack.resolve(stack.argument("value", pos, end));
        while (pos != end) {
            if (val
                != stack.resolve(stack.argument("comparing_value", pos, end))) {
                return fostlib::json{};
            }
        }
        return fostlib::json{val};
    }
    fostlib::json logic_or(
            fsigma::frame &stack,
            fostlib::json::const_iterator pos,
            fostlib::json::const_iterator end) {
        while (pos != end) {
            if (auto ev = stack.resolve(stack.argument("value", pos, end));
                not ev.isnull()) {
                return ev;
            }
        }
        return fostlib::json{};
    }
    fostlib::json
            match(const std::vector<fostlib::string> &args,
                  fsigma::frame &stack,
                  fostlib::json::const_iterator pos,
                  fostlib::json::const_iterator end) {
        auto const arg_idx =
                stack.resolve_int(stack.argument("arg_idx", pos, end));
        if (arg_idx > 0 && arg_idx <= args.size()) {
            return fostlib::json{args[arg_idx - 1]};
        }
        return fostlib::json{};
    }
    fostlib::json sql_exists(
            const fostlib::http::server::request &req,
            const std::vector<fostlib::string> &args,
            fsigma::frame &stack,
            fostlib::json::const_iterator pos,
            fostlib::json::const_iterator end) {
        auto const sql = stack.resolve_string(stack.argument("sql", pos, end));
    }


}


fsigma::frame fostgres::preconditions(
        const fostlib::http::server::request &req, const fostgres::match &m) {
    fsigma::frame f{nullptr};

    f.native["eq"] = [](fsigma::frame &stack, fostlib::json::const_iterator pos,
                        fostlib::json::const_iterator end) {
        return eq(stack, pos, end);
    };
    f.native["header"] = [&req](fsigma::frame &stack,
                                fostlib::json::const_iterator pos,
                                fostlib::json::const_iterator end) {
        return header(req, stack, pos, end);
    };
    f.native["match"] = [&m](fsigma::frame &stack,
                             fostlib::json::const_iterator pos,
                             fostlib::json::const_iterator end) {
        return ::match(m.arguments, stack, pos, end);
    };
    f.native["or"] = [](fsigma::frame &stack, fostlib::json::const_iterator pos,
                        fostlib::json::const_iterator end) {
        return logic_or(stack, pos, end);
    };

    return f;
}
