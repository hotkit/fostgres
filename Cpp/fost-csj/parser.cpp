/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.parser.hpp>
#include <fost/parse/json.hpp>


namespace {
    template<typename P, typename Iter, typename Vec> inline
    void parseline(
        const P &parser, Iter &pos, Iter end, Vec &into
    ) {
        into.clear();
        if ( pos != end ) {
            auto linestr = fostlib::coerce<std::wstring>(*pos);
            auto append_column = [&into](auto v) {
                    into.push_back(v);
                };
            auto parse_result = boost::spirit::parse(
                linestr.begin(), linestr.end(),
                    *boost::spirit::space_p
                    >> parser[append_column]
                    >> *(
                        *boost::spirit::space_p
                        >> boost::spirit::chlit< wchar_t >(L',')
                        >> *boost::spirit::space_p
                        >> parser[append_column])
                    >> *boost::spirit::space_p);
            if ( not parse_result.full ) {
                throw fostlib::exceptions::not_implemented(__func__,
                        "Could not parse row", *pos);
            }
        }
    }
}


/*
    fostlib::cjs::parser
*/


fostlib::csj::parser::parser(utf::u8_view str)
: line_iter(splitter(str, '\n')),
    pos(line_iter.begin()),
    end(line_iter.end())
{
    parseline(fostlib::json_string_parser(), pos, end, headers);
}


fostlib::csj::parser::const_iterator fostlib::csj::parser::begin() const {
    return fostlib::csj::parser::const_iterator(*this, pos);
}


/*
    fostlib::csj::parser::const_iterator
*/


fostlib::csj::parser::const_iterator::const_iterator(
    const parser &o, line_iter_t::const_iterator p
) : owner(o), pos(p) {
    parseline(fostlib::json_p, pos, owner.end, line);
}

