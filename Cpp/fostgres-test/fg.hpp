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


    /// A stack frame
    class frame {
    public:
        using builtin = std::function<fostlib::json(fostlib::ostream &, fostlib::json)>;

        std::shared_ptr<frame> parent;
        std::map<fostlib::string, builtin> native;
        fostlib::json symbols;
    };


    /// Return the builtin functions for the fg environment
    std::shared_ptr<frame> builtins();


    /// A whole program
    class program {
        boost::filesystem::path filename;
        fostlib::json code;
        std::shared_ptr<frame> root;
    public:
        /// Construct an empty program that errors when run
        program();
        /// Parse the requested program
        explicit program(boost::filesystem::path);

        /// Execute this program
        void operator () (fostlib::ostream &) const;

        /// Call a JSON s-expr
    };


}

