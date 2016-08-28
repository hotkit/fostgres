/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/file>


namespace fg {


    using json = fostlib::json;


    /// Parse an fg script and return its JSON representation
    json parse(const boost::filesystem::path &);


    class program;


    /// A stack frame
    class frame {
    public:
        using builtin = std::function<json(frame &, json::const_iterator, json::const_iterator)>;

        frame(const frame *parent);

        const frame *parent;
        std::map<fostlib::string, builtin> native;
        json symbols;

        /// Pop an argument off the head of the args list
        json argument(const fostlib::string &name, json::const_iterator &pos, json::const_iterator end);

        /// Turn an expression into a string
        fostlib::string resolve_string(const json &) const;
        /// Turn an expression into an integer
        int64_t resolve_int(const json &) const;

        /// Resolve a function
        builtin lookup_function(const fostlib::string &name) const;
    };


    /// Return the builtin functions for the fg environment
    frame builtins();


    /// A whole program
    class program {
        boost::filesystem::path filename;
        json code;
    public:
        /// Construct an empty program that errors when run
        program();
        /// Parse the requested program
        explicit program(boost::filesystem::path);

        /// Execute this program
        void operator () (frame &) const;

        /// Script not loaded
        class nothing_loaded : public fostlib::exceptions::exception {
        public:
            nothing_loaded() noexcept;

            fostlib::wliteral const message() const;
        };
        /// Exception for an empty script
        class empty_script : public fostlib::exceptions::exception {
        public:
            empty_script() noexcept;

            fostlib::wliteral const message() const;
        };
    };


    /// Call a JSON s-expr
    json call(frame &parent, const fostlib::json &sexpr);
    /// Call a named function
    json call(frame &parent, const fostlib::string &name,
        json::const_iterator begin, json::const_iterator end);


    namespace lib {
        extern frame::builtin get, put;
    }


}

