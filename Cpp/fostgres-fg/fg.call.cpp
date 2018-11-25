/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.hpp>
#include <fost/push_back>


fg::json fg::call(frame &stack, const json &sexpr) {
    if (not sexpr.isarray()) {
        throw fostlib::exceptions::not_implemented(
                __func__, "Script isn't an array/s-expression", sexpr);
    } else if (sexpr.size() == 0) {
        throw fostlib::exceptions::not_implemented(
                __func__, "The script was empty");
    } else {
        return call(
                stack, stack.resolve_string(*sexpr.begin()), ++sexpr.begin(),
                sexpr.end());
    }
}


fg::json fg::call(
        frame &stack,
        const fostlib::string &name,
        json::const_iterator begin,
        json::const_iterator end) {
    try {
        frame::builtin function(stack.lookup_function(name));
        return function(stack, begin, end);
    } catch (fostlib::exceptions::exception &e) {
        // Built a stack frame
        fg::json sf;
        fostlib::push_back(sf, name);
        for (auto iter = begin; iter != end; ++iter) {
            fostlib::push_back(sf, *iter);
        }
        // Add to the back trace
        fostlib::push_back(e.data(), "fg", "backtrace", sf);
        throw;
    }
}
