/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/split.hpp>
#include <fost/parse/json.hpp>


namespace fostlib {


    namespace csj {


        template<typename Iterator>
        struct headers_parser :
        boost::spirit::qi::grammar<Iterator, std::vector<string>()> {
            using vector_type = std::vector<string>;

            boost::spirit::qi::rule<Iterator, vector_type()> top;
            json_string_parser<Iterator> str;

            headers_parser() : headers_parser::base_type(top) {
                top = str
                        % (*boost::spirit::qi::lit(' ')
                           >> boost::spirit::qi::lit(',')
                           >> *boost::spirit::qi::lit(' '));
            }
        };


        template<typename Iterator>
        struct line_parser :
        boost::spirit::qi::grammar<Iterator, std::vector<json>()> {
            using vector_type = std::vector<json>;

            boost::spirit::qi::rule<Iterator, vector_type()> top;
            json_embedded_parser<Iterator> item;

            line_parser() : line_parser::base_type(top) {
                top = item
                        % (*boost::spirit::qi::lit(' ')
                           >> boost::spirit::qi::lit(',')
                           >> *boost::spirit::qi::lit(' '));
            }
        };


        /// Iterate over a file of CSJ like data
        class parser {
            using line_iter_t = splitter_result<f5::u8view, f5::u8view, 1u>;
            line_iter_t line_iter;
            line_iter_t::const_iterator li_pos, li_end;
            std::vector<fostlib::string> headers;
            headers_parser<f5::const_u32u16_iterator<f5::u8view::const_iterator>>
                    headers_p;
            line_parser<f5::const_u32u16_iterator<f5::u8view::const_iterator>>
                    line_p;

          public:
            /// Initialise from a string
            parser(f5::u8view);

            /// Return the header column names
            const auto &header() const { return headers; }

            class const_iterator {
                friend class parser;
                const parser &owner;
                line_iter_t::const_iterator pos;
                std::vector<fostlib::json> line;

                const_iterator(const parser &, line_iter_t::const_iterator);

              public:
                /// Return the current line
                const std::vector<json> &operator*() const { return line; }
                /// Return the current line as JSON
                json as_json() const;

                /// Move to the next line
                const_iterator &operator++();

                /// Allow comparison
                bool operator==(const_iterator i) const { return pos == i.pos; }
                bool operator!=(const_iterator i) const { return pos != i.pos; }
            };
            friend class const_iterator;

            /// Return iterators
            const_iterator begin() const;
            const_iterator end() const;
        };


    }


}
