/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.hpp>


namespace {
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
    funcs.native["progn"] = progn;
    funcs.native["DELETE"] = lib::del;
    funcs.native["GET"] = lib::get;
    funcs.native["PATCH"] = lib::patch;
    funcs.native["PUT"] = lib::put;
    return funcs;
}

