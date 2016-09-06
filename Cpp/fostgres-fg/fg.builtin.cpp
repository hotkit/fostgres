/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.extension.hpp>
#include <f5/threading/set.hpp>


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


}


fg::frame fg::builtins() {
    frame funcs{nullptr};

    funcs.symbols["testserver.headers"] = fg::json::object_t();

    funcs.native["progn"] = progn;
    funcs.native["DELETE"] = lib::del;
    funcs.native["GET"] = lib::get;
    funcs.native["PATCH"] = lib::patch;
    funcs.native["POST"] = lib::post;
    funcs.native["PUT"] = lib::put;
    funcs.native["sql.file"] = lib::sql_file;

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

