/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.parser.hpp>
#include <fost/parse/json.hpp>
#include <fost/insert>


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
    li_pos(line_iter.begin()),
    li_end(line_iter.end())
{
    parseline(fostlib::json_string_parser(), li_pos, li_end, headers);
    if ( not headers.size() ) {
        throw exceptions::not_implemented(__func__,
            "No headers were found when parsing CSJ");
    }
    ++li_pos;
}


fostlib::csj::parser::const_iterator fostlib::csj::parser::begin() const {
    return fostlib::csj::parser::const_iterator(*this, li_pos);
}
fostlib::csj::parser::const_iterator fostlib::csj::parser::end() const {
    return fostlib::csj::parser::const_iterator(*this, li_end);
}


/*
    fostlib::csj::parser::const_iterator
*/


fostlib::csj::parser::const_iterator::const_iterator(
    const parser &o, line_iter_t::const_iterator p
) : owner(o), pos(p) {
    parseline(fostlib::json_p, pos, owner.li_end, line);
}


fostlib::csj::parser::const_iterator &fostlib::csj::parser::const_iterator::operator ++ () {
    parseline(fostlib::json_p, ++pos, owner.li_end, line);
    if ( not line.size() ) {
        // We've hit a blank line. Make sure we only get them from now on
        while ( pos != owner.li_end ) {
            parseline(fostlib::json_p, ++pos, owner.li_end, line);
            if ( line.size() ) {
                throw exceptions::not_implemented(__func__,
                    "Empty line embedded in CSJ file");
            }
        }
    } else if ( line.size() != owner.header().size() ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "Number of columns didn't match number of headers", line);
    }
    return *this;
}


fostlib::json fostlib::csj::parser::const_iterator::as_json() const {
    fostlib::json row;
    for ( std::size_t c{}; c != line.size(); ++c ) {
        fostlib::insert(row, owner.header()[c], line[c]);
    }
    return row;
}

