/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.iterator.hpp>
#include <fost/parse/json.hpp>


namespace {
    template<typename Iter> inline
    void parseline(Iter &pos, Iter end, std::vector<fostlib::json> &into) {
        into.clear();
        if ( pos != end ) {
            auto linestr = fostlib::coerce<std::wstring>(*pos);
            auto append_column = [&into](auto v) {
                    into.push_back(v);
                };
            auto parse_result = boost::spirit::parse(
                linestr.begin(), linestr.end(),
                    *boost::spirit::space_p
                    >> fostlib::json_p[append_column]
                    >> *(
                        *boost::spirit::space_p
                        >> boost::spirit::chlit< wchar_t >(L',')
                        >> *boost::spirit::space_p
                        >> fostlib::json_p[append_column])
                    >> *boost::spirit::space_p);
            if ( not parse_result.full ) {
                throw fostlib::exceptions::not_implemented(__func__,
                        "Could not parse row", *pos);
            }
        }
    }
}


fostlib::csj::line_iterator::line_iterator(utf::u8_view str)
: line_iter(splitter(str, '\n')),
    pos(line_iter.begin()),
    end(line_iter.end())
{
    parseline(pos, end, line);
}

