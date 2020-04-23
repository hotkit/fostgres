/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/csj.parser.hpp>
#include <fost/insert>
#include <fost/exception/parse_error.hpp>


namespace {
    template<typename P, typename Iter, typename Vec>
    inline void parseline(const P &parser, Iter &pos, Iter end, Vec &into) {
        into.clear();
        if (pos != end) {
            auto line_pos = f5::cord::make_u32u16_iterator(
                    (*pos).begin(), (*pos).end());
            if (not boost::spirit::qi::parse(
                        line_pos.first, line_pos.second, parser, into)
                || line_pos.first != line_pos.second) {
                throw fostlib::exceptions::parse_error{"Could not parse row",
                                                       *pos};
            }
        }
    }
}


/**
    fostlib::cjs::parser
*/


fostlib::csj::parser::parser(f5::u8view str)
: line_iter(splitter(str, '\n')),
  li_pos(line_iter.begin()),
  li_end(line_iter.end()) {
    parseline(headers_p, li_pos, li_end, headers);
    if (not headers.size()) {
        throw exceptions::parse_error{"No headers were found when parsing CSJ"};
    }
    ++li_pos;
}


fostlib::csj::parser::const_iterator fostlib::csj::parser::begin() const {
    return fostlib::csj::parser::const_iterator(*this, li_pos);
}
fostlib::csj::parser::const_iterator fostlib::csj::parser::end() const {
    return fostlib::csj::parser::const_iterator(*this, li_end);
}


/**
    fostlib::csj::parser::const_iterator
*/


fostlib::csj::parser::const_iterator::const_iterator(
        const parser &o, line_iter_t::const_iterator p)
: owner(o), pos(p) {
    parseline(owner.line_p, pos, owner.li_end, line);
}


fostlib::csj::parser::const_iterator &
        fostlib::csj::parser::const_iterator::operator++() {
    parseline(owner.line_p, ++pos, owner.li_end, line);
    if (not line.size()) {
        // We've hit a blank line. Make sure we only get them from now on
        while (pos != owner.li_end) {
            parseline(owner.line_p, ++pos, owner.li_end, line);
            if (line.size()) {
                throw exceptions::parse_error{
                        "Empty line embedded in CSJ file"};
            }
        }
    } else if (line.size() != owner.header().size()) {
        throw fostlib::exceptions::not_implemented(
                __func__, "Number of columns didn't match number of headers",
                line);
    }
    return *this;
}


fostlib::json fostlib::csj::parser::const_iterator::as_json() const {
    fostlib::json row;
    for (std::size_t c{}; c != line.size(); ++c) {
        fostlib::insert(row, owner.header()[c], line[c]);
    }
    return row;
}
