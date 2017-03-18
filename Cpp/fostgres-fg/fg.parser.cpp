/*
    Copyright 2016-2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.hpp>
#include <fostgres/fg/parser.hpp>

#include <fost/exception/parse_error.hpp>
#include <fost/push_back>


fostlib::json fg::parse(const boost::filesystem::path &filename) {
    fostlib::string code(fostlib::utf::load_file(filename));
    std::vector<fostlib::json> script;
    auto pos = f5::make_u32u16_iterator(code.begin(), code.end());
    fg_parser<decltype(pos.first)> fg_p;
    if ( boost::spirit::qi::parse(pos.first, pos.second, fg_p, script) && pos.first == pos.second ) {
        fostlib::json::array_t ret;
        ret.push_back(fostlib::json("progn"));
        for ( auto &&line : script ) {
            ret.push_back(line);
        }
        return fostlib::json(ret);
    } else {
        throw fostlib::exceptions::parse_error("Could not parse FG script",
            fostlib::string(pos.first.u32_iterator(), pos.second.u32_iterator()));
    }
}

