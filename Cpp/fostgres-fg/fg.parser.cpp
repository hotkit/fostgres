/**
    Copyright 2016-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fostgres/fg/fg.hpp>
#include <fostgres/fg/parser.hpp>

#include <fost/exception/parse_error.hpp>
#include <fost/push_back>


fostlib::json fg::parse(f5::u8view code) {
    std::vector<fostlib::json> script;
    auto pos = f5::cord::make_u32u16_iterator(code.begin(), code.end());
    fg_parser<decltype(pos.first)> fg_p;
    if (boost::spirit::qi::parse(pos.first, pos.second, fg_p, script)
        && pos.first == pos.second) {
        fostlib::json::array_t ret;
        ret.push_back(fostlib::json("progn"));
        for (auto &&line : script) { ret.push_back(line); }
        return fostlib::json(ret);
    } else {
        throw fostlib::exceptions::parse_error(
                "Could not parse FG script",
                fostlib::string(
                        pos.first.u32_iterator(), pos.second.u32_iterator()));
    }
}
