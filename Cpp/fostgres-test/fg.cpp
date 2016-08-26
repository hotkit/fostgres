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


fg::program::program()
: root(builtins()) {
}


fg::program::program(boost::filesystem::path fn)
: filename(std::move(fn)), code(parse(filename)), root(builtins()) {
}


void fg::program::operator () (fostlib::ostream &o) const {
    if ( not code.isarray() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "No script has been loaded", code);
    } else if ( code.size() <= 1 ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "The script was empty");
    } else {
        call(o, root, code);
    }
}

