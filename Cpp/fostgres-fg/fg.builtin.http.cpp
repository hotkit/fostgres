/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/mime.hpp>
#include <fostgres/fg/fg.hpp>
#include <fostgres/fg/fg.testserver.hpp>


namespace {


    fg::json nobody(
        std::function<
            std::pair<boost::shared_ptr<fostlib::mime>, int>(
                fg::testserver&, fg::frame&, const fostlib::string &)> op,
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto viewname = stack.resolve_string(stack.argument("view", pos, end));
        auto path = stack.resolve_string(stack.argument("path", pos, end));
        auto status = stack.resolve_int(stack.argument("status", pos, end));
        fg::testserver server(stack, viewname);
        auto actual = op(server, stack, path);
        if ( actual.second != status ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "Actual resopnse status isn't what was epected", actual.second);
        }
        if ( pos != end ) {
            auto response = fg::mime_from_argument(stack, stack.argument("response", pos, end));
            fg::assert_comparable(*actual.first, *response);
        }
        return fostlib::json();
    }
    fg::json withbody(
        std::function<
            std::pair<boost::shared_ptr<fostlib::mime>, int>(
                fg::testserver&, fg::frame&, const fostlib::string &, const fg::json &)> op,
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto viewname = stack.resolve_string(stack.argument("view", pos, end));
        auto path = stack.resolve_string(stack.argument("path", pos, end));
        auto body = stack.argument("body", pos, end);
        auto status = stack.resolve_int(stack.argument("status", pos, end));
        fg::testserver server(stack, viewname);
        auto actual = op(server, stack, path, body);
        if ( actual.second != status ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "Actual resopnse status isn't what was epected", actual.second);
        }
        if ( pos != end ) {
            auto response = fg::mime_from_argument(stack, stack.argument("response", pos, end));
            fg::assert_comparable(*actual.first, *response);
        }
        return fostlib::json();
    }


    fg::json get(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        return nobody(&fg::testserver::get, stack, pos, end);
    }
    fg::json put(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        return withbody(&fg::testserver::put, stack, pos, end);
    }
    fg::json patch(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        return withbody(&fg::testserver::patch, stack, pos, end);
    }
    fg::json post(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        return withbody(&fg::testserver::post, stack, pos, end);
    }
    fg::json del(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        return nobody(&fg::testserver::del, stack, pos, end);
    }


}


fg::frame::builtin fg::lib::get = ::get;
fg::frame::builtin fg::lib::put = ::put;
fg::frame::builtin fg::lib::patch = ::patch;
fg::frame::builtin fg::lib::post = ::post;
fg::frame::builtin fg::lib::del = ::del;

