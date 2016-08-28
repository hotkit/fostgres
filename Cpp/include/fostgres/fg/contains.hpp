/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/core>


namespace fg {


    /// Returns true if all parts of `sub` exist and have the same
    /// value as the same location in `super`. Objects match if all
    /// keys in `sub` are present in `super` and the values match.
    /// Arrays match if each element matches, although `sub` may
    /// be shorter. All other valuess (atoms) must be the same,
    /// although a null in sub means that the key doesn't have to
    /// exist in `super`.
    fostlib::nullable<fostlib::jcursor> contains(const fostlib::json &super, const fostlib::json &sub);


}

