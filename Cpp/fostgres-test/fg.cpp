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


fg::program::program(const boost::filesystem::path &filename) {
    throw fostlib::exceptions::not_implemented(__func__);
}


void fg::program::operator () () const {
    throw fostlib::exceptions::not_implemented(__func__);
}

