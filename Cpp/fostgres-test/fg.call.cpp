/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"


fg::json fg::call(frame &stack, const json &sexpr) {
    if ( not sexpr.isarray() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "Script isn't an array/s-expression", sexpr);
    } else if ( sexpr.size() == 0 ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "The script was empty");
    } else {
        return call(stack, stack.resolve_string(*sexpr.begin()), ++sexpr.begin(), sexpr.end());
    }
}


fg::json fg::call(
    frame &stack,
    const fostlib::string &name, json::const_iterator begin, json::const_iterator end
) {
    frame::builtin function(stack.lookup_function(name));
    return function(stack, begin, end);
}

