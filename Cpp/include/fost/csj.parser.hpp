/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/split.hpp>


namespace fostlib {


    namespace csj {


        /// Iterate over a file of CSJ like data
        class parser {
            using line_iter_t = splitter_result<utf::u8_view, utf::u8_view>;
            line_iter_t line_iter;
            line_iter_t::const_iterator pos, end;
            std::vector<fostlib::string> headers;
        public:
            /// Initialise from a string
            parser(utf::u8_view);

            class const_iterator {
                friend class parser;
                const parser &owner;
                line_iter_t::const_iterator pos;
                std::vector<fostlib::json> line;

                const_iterator(const parser &, line_iter_t::const_iterator);
            public:
                /// Return the current line
                const std::vector<json> &operator * () const {
                    return line;
                }
            };
            friend class const_iterator;

            const_iterator begin() const;
        };


    }


}

