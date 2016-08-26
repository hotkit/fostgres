/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"


/*
    fg::program
 */


fg::program::program() {
}


fg::program::program(boost::filesystem::path fn)
: filename(std::move(fn)), code(parse(filename)) {
}


void fg::program::operator () (fostlib::ostream &o) const {
    if ( not code.isarray() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "No script has been loaded", code);
    } else if ( code.size() <= 1 ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "The script was empty");
    } else {
        frame stack(builtins());
        stack.native["module.path.join"] = [this](
                fostlib::ostream &o, fg::frame &stack,
                fg::json::const_iterator pos, fg::json::const_iterator end
            ) {
                auto path = fostlib::coerce<boost::filesystem::path>(
                    stack.resolve_string(stack.argument("path", pos, end)));
                auto result = fostlib::join_paths(filename.parent_path(), path);
                return fostlib::coerce<fostlib::json>(result);
            };
        call(o, stack, code);
    }
}

