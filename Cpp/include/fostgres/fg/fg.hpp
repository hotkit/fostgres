/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fost/file>
#include <fostgres/fsigma.hpp>


namespace fg {


    const extern fostlib::module c_fg;


    using frame = fsigma::frame;
    using json = fostlib::json;
    using jcursor = fostlib::jcursor;


    /// Parse an fg script and return its JSON representation
    json parse(f5::u8view);


    /// Return the builtin functions for the fg environment
    frame builtins();


    /// A whole program
    class program {
        fostlib::fs::path filename;
        f5::u8string source;
        json code;

      public:
        /// Construct an empty program that errors when run
        program();
        /// Parse the requested program
        explicit program(fostlib::fs::path);

        /// Execute this program
        void operator()(frame &) const;

        /// Return the source code line for the requested string, if available
        struct location {
            fostlib::fs::path const &filename;
            f5::u8string source;
            std::size_t line, column;
        };
        std::optional<location> source_for(f5::u8view) const;

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


    namespace lib {
        extern frame::builtin contains, del, get, head, patch, post, put,
                sql_file, sql_insert;
    }


}
