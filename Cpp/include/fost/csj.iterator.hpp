/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/core>


namespace fostlib {


    namespace csj {


        /// Iterate over a file of CSJ like data
        class line_iterator {
            std::vector<fostlib::json> line;
        public:
            /// End iterator
            line_iterator();

            /// Initialise from a string
            line_iterator(const utf8_string &);

            /// Return the current line
            const std::vector<json> operator * () const {
                return line;
            }
        };


    }


}

