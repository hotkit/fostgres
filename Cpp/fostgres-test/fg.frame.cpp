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


fostlib::string fg::frame::resolve_string(const json &code) const {
    if ( code.isatom() ) {
        return fostlib::coerce<fostlib::string>(code);
    } else {
        throw fostlib::exceptions::not_implemented(__func__,
            "Can't resolve to a string", code);
    }
}


fg::frame::builtin fg::frame::resolve_function(const fostlib::string &name) const {
    auto fnp = native.find(name);
    if ( fnp == native.end() ) {
        if ( parent ) {
            return parent->resolve_function(name);
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "Function not found", name);
        }
    } else {
        return fnp->second;
    }
}

