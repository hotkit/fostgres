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


fg::frame::frame(const frame *f)
: parent(f) {
}


fg::json fg::frame::argument(
    const fostlib::string &name, json::const_iterator &pos, json::const_iterator end
) {
    if ( pos == end ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "Argument not found", name);
    } else {
        auto result = *pos;
        ++pos;
        return result;
    }
}


fostlib::string fg::frame::resolve_string(const json &code) const {
    if ( code.isatom() ) {
        return fostlib::coerce<fostlib::string>(code);
    } else if ( code.isarray() ) {
        frame stack(this);
        return resolve_string(call(stack, code));
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "Can't resolve to a string", code);
    }
}


int64_t fg::frame::resolve_int(const json &code) const {
    if ( code.isatom() ) {
        return fostlib::coerce<int64_t>(code);
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "Can't resolve to an int", code);
    }
}


/// This is dynamic rather than lexical scoping, which is.... not great
fg::frame::builtin fg::frame::lookup_function(const fostlib::string &name) const {
    auto fnp = native.find(name);
    if ( fnp == native.end() ) {
        if ( parent ) {
            return parent->lookup_function(name);
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "Function not found", name);
        }
    } else {
        return fnp->second;
    }
}

