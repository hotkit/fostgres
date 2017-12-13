/*
    Copyright 2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/parse/json.hpp>


namespace fg {


    template<typename Iterator>
    struct fg_parser : boost::spirit::qi::grammar<Iterator, std::vector<fostlib::json>()> {
        boost::spirit::qi::rule<Iterator, std::vector<fostlib::json>()> top, inner_sexpr;
        boost::spirit::qi::rule<Iterator, void()> space, newline, comment, linebreak;
        boost::spirit::qi::rule<Iterator, std::string()> comment_p, identifier_seq;
        boost::spirit::qi::rule<Iterator, fostlib::json()> identifier, inner_sexpr_json, full_sexpr;
        fostlib::json_embedded_parser<Iterator> json_p;

        fg_parser()
        : fg_parser::base_type(top) {
            namespace qi = boost::spirit::qi;

            space = qi::lit(' ');
            newline = qi::lit('\n');

            comment_p = qi::lit('#') >> *(qi::standard_wide::char_ - newline);
            comment = qi::omit[comment_p];

            linebreak = *space >> -comment >> newline;

            identifier_seq= +(qi::standard_wide::char_ - (space | newline | qi::lit(')') | qi::lit('#')));
            identifier = identifier_seq[boost::phoenix::bind([](auto &v, auto &s) {
                    v = fostlib::json(s);
                }, qi::_val, qi::_1)];

            inner_sexpr = (json_p.object | json_p.atom | full_sexpr | identifier) % +space;
            inner_sexpr_json = inner_sexpr[boost::phoenix::bind([](auto &v, auto &a) {
                    fostlib::json::array_t arr;
                    for ( auto &&l : a ) {
                        arr.push_back(l);
                    }
                    v = arr;
                }, qi::_val, qi::_1)];
            full_sexpr = qi::lit('(') >> *space >> inner_sexpr_json >> *space >> qi::lit(')');

            top = qi::omit[*linebreak] >> -(inner_sexpr_json % +linebreak) >> qi::omit[*linebreak];
        }
    };


}

