/*
    Copyright 2016-2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/contains.hpp>
#include <fost/test>


namespace {
    fostlib::nullable<fostlib::jcursor> walk(
        fostlib::jcursor &path, const fostlib::json &super, const fostlib::json &sub
    ) {
        if ( sub.isnull() || sub.isatom() || sub.isarray() ) {
            if ( super[path] == sub ) {
                return fostlib::null;
            } else {
                return path;
            }
        } else if ( sub.isobject() ) {
            for ( fostlib::json::const_iterator p(sub.begin()); p != sub.end(); ++p ) {
                path /= p.key();
                if ( super.has_key(path) ) {
                    auto part = walk(path, super, *p);
                    if ( part ) return path;
                } else {
                    if ( *p != fostlib::json() ) {
                        return path;
                    }
                }
                path.pop();
            }
        } else {
            throw fostlib::exceptions::not_implemented(__func__,
                "Can't walk across ths sub-object", sub);
        }
        return fostlib::null;
    }
}


fostlib::nullable<fostlib::jcursor> fg::contains(const fg::json &super, const fg::json &sub) {
    fostlib::jcursor path;
    return walk(path, super, sub);
}


void fg::throw_contains_error(fg::json actual, fg::json expected, fg::jcursor contains) {
    fostlib::exceptions::test_failure error("Mismatched response body", __FILE__, __LINE__);
    fostlib::insert(error.data(), "expected", expected);
    fostlib::insert(error.data(), "actual", actual);
    fostlib::insert(error.data(), "mismatch", "path", contains);
    fostlib::insert(error.data(), "mismatch", "expected", expected[contains]);
    fostlib::insert(error.data(), "mismatch", "actual", actual[contains]);
    throw error;
}

