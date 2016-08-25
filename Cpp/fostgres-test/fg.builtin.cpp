/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"


std::shared_ptr<fg::frame> fg::builtins() {
    auto funcs = std::make_shared<frame>();
    return funcs;
}

