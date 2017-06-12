/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.parser.hpp>
#include <fost/insert>


namespace {
    template<typename P, typename Iter, typename Vec> inline
    void parseline(
        const P &parser, Iter &pos, Iter end, Vec &into
    ) {
        into.clear();
        if ( pos != end && not (*pos).empty() ) {
            auto line_pos = f5::make_u32u16_iterator((*pos).begin(), (*pos).end());
            if ( not boost::spirit::qi::parse(line_pos.first, line_pos.second, parser, into)
                    || line_pos.first != line_pos.second )
            {
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
    reset();
}


void fostlib::csj::parser::reset() {
    while ( (*li_pos).empty() && li_pos != li_end )
        ++li_pos;
    parseline(headers_p, li_pos, li_end, headers);
    if ( not headers.size() ) {
        throw exceptions::not_implemented(__func__,
            "No headers were found when parsing CSJ");
    }
    ++li_pos;
}


fostlib::csj::parser::const_iterator fostlib::csj::parser::begin() const {
    return fostlib::csj::parser::const_iterator(*this, li_pos, false);
}
fostlib::csj::parser::const_iterator fostlib::csj::parser::end() const {
    return fostlib::csj::parser::const_iterator(*this, li_end, true);
}


/*
    fostlib::csj::parser::const_iterator
*/


fostlib::csj::parser::const_iterator::const_iterator(
    const parser &o, line_iter_t::const_iterator p, bool end_iter
) : owner(o), pos(p), end_iterator(end_iter) {
    parseline(owner.line_p, pos, owner.li_end, line);
}


fostlib::csj::parser::const_iterator &fostlib::csj::parser::const_iterator::operator ++ () {
    parseline(owner.line_p, ++pos, owner.li_end, line);
    if ( not line.size() ) {
        // We've (probably) hit a blank line. Make sure we only get them
        // from now on
        while ( pos != owner.li_end ) {
            parseline(owner.line_p, ++pos, owner.li_end, line);
            if ( line.size() ) {
                throw exceptions::not_implemented(__func__,
                    "Empty line embedded in CSJ file", line);
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


/*
    fostlib::csj::multi_parser
*/


fostlib::csj::multi_parser::multi_parser(utf::u8_view b)
: current(b) {
}


auto fostlib::csj::multi_parser::begin() const -> const_iterator {
    return const_iterator{*this};
}


auto fostlib::csj::multi_parser::end() const -> const_iterator {
    return const_iterator{*this};
}


/*
    fostlib::csj::multi_parser::const_iterator
*/


auto fostlib::csj::multi_parser::const_iterator::operator ++ () -> const_iterator & {
    current.reset();
    return *this;
}


bool fostlib::csj::multi_parser::const_iterator::operator == (const const_iterator &i) const {
    return false;
}

