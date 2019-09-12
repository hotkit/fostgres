/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/core>


namespace fsigma {


    /// A stack frame
    class frame {
      public:
        using builtin = std::function<fostlib::json(
                frame &,
                fostlib::json::const_iterator,
                fostlib::json::const_iterator)>;

        frame(frame *parent);

        frame *parent;
        std::map<fostlib::string, builtin> native;
        std::map<fostlib::string, fostlib::json> symbols;

        /// Pop an argument off the head of the args list
        fostlib::json argument(
                const fostlib::string &name,
                fostlib::json::const_iterator &pos,
                fostlib::json::const_iterator end);

        /// Turn an expression into a string
        fostlib::string resolve_string(const fostlib::json &);
        /// Turn an expression into an integer
        int64_t resolve_int(const fostlib::json &);
        /// Expect that the JSON represents executable code
        fostlib::json resolve(const fostlib::json &);

        /// Lookup a symbol
        fostlib::json lookup(const fostlib::string &name) const;
        /// Resolve a function
        builtin lookup_function(const fostlib::string &name) const;
    };


    /// Call a JSON s-expr
    fostlib::json call(frame &parent, const fostlib::json &sexpr);
    /// Call a named function
    fostlib::json
            call(frame &parent,
                 const fostlib::string &name,
                 fostlib::json::const_iterator begin,
                 fostlib::json::const_iterator end);


}
