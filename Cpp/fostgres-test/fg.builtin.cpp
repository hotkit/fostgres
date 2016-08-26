/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"


namespace {
    fg::json progn(
        fostlib::ostream &o, fg::frame &, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        int64_t executed = 0u;
        for ( ; pos != end; ++ pos ) {
            o << *pos << std::endl;
            throw fostlib::exceptions::not_implemented(__func__);
        }
        return fg::json(executed);
    }
}


fg::frame fg::builtins() {
    frame funcs{nullptr};
    funcs.native["progn"] = progn;
    return funcs;
}

