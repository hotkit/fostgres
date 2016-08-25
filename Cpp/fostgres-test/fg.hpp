/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/file>


namespace fg {


    /// Parse an fg script and return its JSON representation
    fostlib::json parse(const boost::filesystem::path &);


    class program {
        boost::filesystem::path filename;
        fostlib::json code;
    public:
        /// Construct an empty program that errors when run
        program();
        /// Parse the requested program
        explicit program(boost::filesystem::path);

        /// Execute this program
        void operator () () const;
    };


}

