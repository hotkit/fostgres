/*
    Copyright 2016-2017 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.hpp>

#include <fostgres/fostgres.hpp>


const fostlib::module fg::c_fg(fostgres::c_fostgres, "fg");


/*
    fg::program
 */


fg::program::program() {
}


fg::program::program(boost::filesystem::path fn)
: filename(std::move(fn)), code(parse(filename)) {
}


void fg::program::operator () (frame &stack) const {
    if ( not code.isarray() ) {
        throw nothing_loaded();
    } else if ( code.size() <= 1 ) {
        throw empty_script();
    } else {
        stack.native["module.path.join"] = [this](
                fg::frame &stack,
                fg::json::const_iterator pos, fg::json::const_iterator end
            ) {
                auto path = fostlib::coerce<boost::filesystem::path>(
                    stack.resolve_string(stack.argument("path", pos, end)));
                auto result = fostlib::join_paths(filename.parent_path(), path);
                return fostlib::coerce<fostlib::json>(result);
            };
        call(stack, code);
    }
}


/*
    fg::program::empty_script
 */


fg::program::empty_script::empty_script() noexcept {
}


fostlib::wliteral const fg::program::empty_script::message() const {
    return L"Empty script";
}


/*
    fg::program::nothing_loaded
 */


fg::program::nothing_loaded::nothing_loaded() noexcept {
}


fostlib::wliteral const fg::program::nothing_loaded::message() const {
    return L"No script has been loaded";
}

