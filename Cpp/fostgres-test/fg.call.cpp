/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"


fg::json fg::call(
    fostlib::ostream &o, const frame &parent,
    const fostlib::string &name, json::const_iterator begin, json::const_iterator end
) {
    frame stack(&parent);
    frame::builtin function(stack.resolve_function(name));
    return function(o, stack, begin, end);
}

