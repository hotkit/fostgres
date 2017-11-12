/*
    Copyright 2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/contains.hpp>
#include <fostgres/fg/fg.hpp>


fg::frame::builtin fg::lib::contains =
    [](fg::frame &stack, fg::json::const_iterator pos, fg::json::const_iterator end) {
        auto data = stack.resolve(stack.argument("data", pos, end));
        auto check = stack.resolve(stack.argument("check", pos, end));
        auto result = fg::contains(data, check);
        if ( result ) {
            throw_contains_error(data, check, result.value());
        }
        return data;
    };

