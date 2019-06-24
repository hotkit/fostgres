/**
    Copyright 2016-2018 Felspar Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fostgres/fg/mime.hpp>
#include <fostgres/fg/fg.hpp>
#include <fostgres/fg/fg.testserver.hpp>

#include <fost/unicode>


namespace {


    auto body_data(const fostlib::mime &body) {
        std::vector<unsigned char> data;
        for (const auto &part : body) {
            data.insert(
                    data.end(), reinterpret_cast<const char *>(part.first),
                    reinterpret_cast<const char *>(part.second));
        }
        return fostlib::json{f5::u8view(data)};
    }


    template<typename O>
    inline fg::json
            nobody(O op,
                   fg::frame &stack,
                   fg::json::const_iterator pos,
                   fg::json::const_iterator end) {
        auto viewname = stack.resolve_string(stack.argument("view", pos, end));
        auto path = stack.resolve_string(stack.argument("path", pos, end));
        auto status = stack.resolve_int(stack.argument("status", pos, end));
        fg::testserver server(stack, viewname);
        auto actual = (server.*op)(stack, path, status);
        if (actual.second != status) {
            throw fostlib::exceptions::not_implemented(
                    __func__, "Actual response status isn't what was expected",
                    actual.second);
        }
        if (pos != end) {
            auto response = fg::mime_from_argument(
                    stack, stack.argument("response", pos, end));
            fg::assert_comparable(*actual.first, *response);
            return fostlib::json();
        }
        return body_data(*actual.first);

    }
    template<typename O>
    inline fg::json withbody(
            O op,
            fg::frame &stack,
            fg::json::const_iterator pos,
            fg::json::const_iterator end) {
        auto viewname = stack.resolve_string(stack.argument("view", pos, end));
        auto path = stack.resolve_string(stack.argument("path", pos, end));
        auto body =
                fg::mime_from_argument(stack, stack.argument("body", pos, end));
        auto status = stack.resolve_int(stack.argument("status", pos, end));
        fg::testserver server(stack, viewname);
        auto actual = (server.*op)(stack, path, std::move(body), status);
        if (actual.second != status) {
            throw fostlib::exceptions::not_implemented(
                    __func__, "Actual response status isn't what was expected",
                    actual.second);
        }
        if (pos != end) {
            auto response = fg::mime_from_argument(
                    stack, stack.argument("response", pos, end));
            fg::assert_comparable(*actual.first, *response);
        }
        return body_data(*actual.first);
    }


    fg::json
            get(fg::frame &stack,
                fg::json::const_iterator pos,
                fg::json::const_iterator end) {
        return nobody(&fg::testserver::get, stack, pos, end);
    }
    fg::json
            put(fg::frame &stack,
                fg::json::const_iterator pos,
                fg::json::const_iterator end) {
        return withbody(&fg::testserver::put, stack, pos, end);
    }
    fg::json
            patch(fg::frame &stack,
                  fg::json::const_iterator pos,
                  fg::json::const_iterator end) {
        return withbody(&fg::testserver::patch, stack, pos, end);
    }
    fg::json
            post(fg::frame &stack,
                 fg::json::const_iterator pos,
                 fg::json::const_iterator end) {
        return withbody(&fg::testserver::post, stack, pos, end);
    }
    fg::json
            del(fg::frame &stack,
                fg::json::const_iterator pos,
                fg::json::const_iterator end) {
        return nobody(&fg::testserver::del, stack, pos, end);
    }


}


fg::frame::builtin fg::lib::get = ::get;
fg::frame::builtin fg::lib::put = ::put;
fg::frame::builtin fg::lib::patch = ::patch;
fg::frame::builtin fg::lib::post = ::post;
fg::frame::builtin fg::lib::del = ::del;
