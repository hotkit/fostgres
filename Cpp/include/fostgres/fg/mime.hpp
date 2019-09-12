/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fostgres/fg/fg.hpp>
#include <fost/urlhandler>


namespace fg {


    /// Load a MIME from a filename
    std::unique_ptr<fostlib::binary_body>
            mime_from_argument(frame &stack, const json &);


    /// Compare two MIME results
    void assert_comparable(
            const fostlib::mime &actual, const fostlib::mime &expected);


}
