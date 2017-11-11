/*
    Copyright 2016-2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.extension.hpp>
#include <f5/threading/set.hpp>

#include <fost/log>


namespace {


    f5::tsset<fg::register_builtins*> g_registrations;


    fg::json progn(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        int64_t executed = 0u;
        for ( ; pos != end; ++ pos ) {
            call(stack, *pos);
        }
        return fg::json(executed);
    }


    fg::json cat(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        fostlib::string catted;
        for ( ; pos != end; ) {
            catted += stack.resolve_string(stack.argument("string", pos, end));
        }
        return fg::json(catted);
    }


    /// Return the current binding (if there is no binding then error)
    fg::json lookup(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto name = stack.resolve_string(stack.argument("varname", pos, end));
        auto value = stack.lookup(name);
        fostlib::log::debug(fg::c_fg)
            ("", "Symbol value")
            ("varname", name)
            ("value", value);
        return value;
    }


    /// Bind a name to a value
    fg::json set(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto name = stack.resolve_string(stack.argument("varname", pos, end));
        auto value = stack.resolve(stack.argument("value", pos, end));
        fostlib::log::debug(fg::c_fg)
            ("", "set a value")
            ("name", name)
            ("value", value);
        if ( stack.parent ) {
            stack.parent->symbols[name] = value;
        } else {
            stack.symbols[name] = value;
        }
        return value;
    }
    /// Set a sub-value in the JSON at this symbol
    fg::json set_path(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto name = stack.resolve_string(stack.argument("varname", pos, end));
        auto path = stack.argument("path", pos, end);
        auto value = stack.resolve(stack.argument("value", pos, end));
        stack.symbols[name] =
            fostlib::coerce<fostlib::jcursor>(path).set(stack.symbols[name], value);
        fostlib::log::debug(fg::c_fg)
            ("", "set a value")
            ("name", name)
            ("path", path)
            ("value", value)
            ("becomes", stack.symbols[name]);
        return value;
    }


    /// Change the value of a setting
    fg::json setting(
        fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        auto domain = stack.resolve_string(stack.argument("domain", pos, end));
        auto name = stack.resolve_string(stack.argument("name", pos, end));
        auto value = stack.argument("value", pos, end);
        // Argh, fostlib::setting should be movable
        static std::vector<std::unique_ptr<fostlib::setting<fg::json>>> settings;
        settings.push_back(
            std::make_unique<fostlib::setting<fg::json>>(
                "fostgres-test", domain, name, value));
        return value;
    }


}


fg::frame fg::builtins() {
    frame funcs{nullptr};

    funcs.symbols["testserver.headers"] = fg::json::object_t();

    funcs.native["cat"] = ::cat;
    funcs.native["DELETE"] = lib::del;
    funcs.native["GET"] = lib::get;
    funcs.native["lookup"] = ::lookup;
    funcs.native["PATCH"] = lib::patch;
    funcs.native["POST"] = lib::post;
    funcs.native["progn"] = ::progn;
    funcs.native["PUT"] = lib::put;
    funcs.native["set"] = ::set;
    funcs.native["set-path"] = ::set_path;
    funcs.native["setting"] = ::setting;
    funcs.native["sql.file"] = lib::sql_file;
    funcs.native["sql.insert"] = lib::sql_insert;

    g_registrations.for_each(
        [&funcs](auto *f) {
                (*f)(funcs);
            });

    return funcs;
}


/*
    fg::register_builtins
*/


fg::register_builtins::register_builtins(std::function<void(frame &)> lambda)
: lambda(lambda) {
    g_registrations.insert_if_not_found(this);
}


void fg::register_builtins::operator () (frame &f) const {
    lambda(f);
}

