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
    if ( code.size() == 0 ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "No script has loaded, or the script was empty");
    } else if ( not code.isarray() ) {
            throw fostlib::exceptions::not_implemented(__func__,
                "The script is not a JSON array for some reason", code);
    } else {
        for ( const auto &line : code ) {
            o << line << std::endl;
        }
    }
    throw fostlib::exceptions::not_implemented(__func__);
}

