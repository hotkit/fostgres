/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"
#include "fg.testserver.hpp"


namespace {
    fg::json put(
        fostlib::ostream &o, fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end
    ) {
        throw fostlib::exceptions::not_implemented(__func__);
    }
}


fg::frame::builtin fg::lib::put = ::put;

