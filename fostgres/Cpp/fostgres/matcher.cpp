/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/matcher.hpp>
#include <fost/string>


namespace {
    fostlib::nullable<fostgres::match> check(
        const fostlib::json &o, const fostlib::split_type &parts
    ) {
        fostgres::match m{o};
        if ( o.has_key("path") ) {
            for ( auto nj : o["path"] ) {
                auto n = fostlib::coerce<int>(nj);
                if ( n <= parts.size() ) {
                    m.arguments.push_back(parts[n - 1]);
                } else {
                    return fostlib::null;
                }
            }
        }
        return m;
    }
}


fostlib::nullable<fostgres::match> fostgres::matcher(
    const fostlib::json &configuration, const fostlib::string &path
) {
    auto parts = fostlib::split(path, "/");
    if ( configuration.isobject() ) {
        return check(configuration, parts);
    } else if ( configuration.isarray() ) {
        for ( auto conf : configuration ) {
            auto m = check(conf, parts);
            if ( not m.isnull() ) return m;
        }
    }
    return fostlib::null;
}

