/**
    Copyright 2017-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/parse/json.hpp>


/**

We really want the `identity_seq` below (and any other strings) to be
`f5::u8string` instances which have been created from an iterator pair
from the source string. This would allow us to recognise that these strings
are related, and it would also save us allocations for the sub-stings.

Unfortunately it seems that Spirit doesn't work that way. It insists on
adding to the strings one character at a time.

```cpp
namespace  boost::spirit::traits {
    template<>
    struct is_string<f5::u8string> {
        using type = boost::mpl::true_;
    };
    template<>
    struct char_type_of<f5::u8string> {
        using type = char;
    };
    template<>
    struct extract_c_string<f5::u8string> {
        using char_type = char;
        static char_type const *call (f5::u8string &s) {
            return s.shrink_to_fit();
        }
    };
}
```
 */


namespace fg {


    template<typename Iterator>
    struct fg_parser :
    boost::spirit::qi::grammar<Iterator, std::vector<fostlib::json>()> {
        boost::spirit::qi::rule<Iterator, std::vector<fostlib::json>()> top,
                inner_sexpr;
        boost::spirit::qi::rule<Iterator, void()> space, newline, comment,
                linebreak;
        boost::spirit::qi::rule<Iterator, std::string()> comment_p,
                identifier_seq;
        boost::spirit::qi::rule<Iterator, fostlib::json()> identifier,
                inner_sexpr_json, full_sexpr, quoted_json;
        fostlib::json_embedded_parser<Iterator> json_p;

        fg_parser() : fg_parser::base_type(top) {
            namespace qi = boost::spirit::qi;

            space = qi::lit(' ');
            newline = qi::lit('\n');

            comment_p = qi::lit('#') >> *(qi::standard_wide::char_ - newline);
            comment = qi::omit[comment_p];

            linebreak = *space >> -comment >> newline;

            identifier_seq =
                    +(qi::standard_wide::char_
                      - (space | newline | qi::lit(')') | qi::lit('#')));
            identifier = identifier_seq[boost::phoenix::bind(
                    [](auto &v, auto &s) { v = fostlib::json(s); }, qi::_val,
                    qi::_1)];

            quoted_json = json_p.array[boost::phoenix::bind(
                    [](auto &v, auto &a) {
                        fostlib::json::array_t arr;
                        arr.push_back(fostlib::json("quote"));
                        arr.push_back(a);
                        v = fostlib::json(std::move(arr));
                    },
                    qi::_val, qi::_1)];

            inner_sexpr = (json_p.object | quoted_json | json_p.atom
                           | full_sexpr | identifier)
                    % +space;
            inner_sexpr_json = inner_sexpr[boost::phoenix::bind(
                    [](auto &v, auto &a) {
                        fostlib::json::array_t arr;
                        for (auto &&l : a) { arr.push_back(l); }
                        v = arr;
                    },
                    qi::_val, qi::_1)];
            full_sexpr = qi::lit('(') >> *space >> inner_sexpr_json >> *space
                    >> qi::lit(')');

            top = qi::omit[*linebreak] >> -(inner_sexpr_json % +linebreak)
                    >> qi::omit[*linebreak];
        }
    };


}
