/**
    Copyright 2016-2019 Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fostgres/fg/fg.hpp>

#include <fostgres/fostgres.hpp>


const fostlib::module fg::c_fg(fostgres::c_fostgres, "fg");


/**
    ## fg::program
 */

namespace {
    f5::u8string load(fostlib::fs::path const &fn) {
        return fostlib::utf::load_file(fn).u8string_transition();
    }
}


fg::program::program() {}


fg::program::program(fostlib::fs::path fn)
: filename{std::move(fn)}, source{::load(filename)}, code{parse(source)} {}


void fg::program::operator()(frame &stack) const {
    if (not code.isarray()) {
        throw nothing_loaded();
    } else if (code.size() <= 1) {
        throw empty_script();
    } else {
        stack.native["module.path.join"] = [this](fg::frame &stack,
                                                  fg::json::const_iterator pos,
                                                  fg::json::const_iterator end) {
            auto path = fostlib::coerce<fostlib::fs::path>(
                    stack.resolve_string(stack.argument("path", pos, end)));
            auto result = fostlib::join_paths(filename.parent_path(), path);
            return fostlib::coerce<fostlib::json>(result);
        };
        call(stack, code);
    }
}


auto fg::program::source_for(f5::u8view s) const -> std::optional<location> {
    if(s.shares_allocation_with(source)) {
        /// This can never happen because of the way boost::spirit works.
        /// See the note in [parser.hpp](../include/fostgres/fg/parser.hpp)
        throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__,
            "This code has (until now) been impossible to reach");
        //return location{filename, s, 1, 1};
    } else {
        return {};
    }
}


/**
    ## fg::program::empty_script
 */


fg::program::empty_script::empty_script() noexcept {}


fostlib::wliteral const fg::program::empty_script::message() const {
    return L"Empty script";
}


/**
    ## fg::program::nothing_loaded
 */


fg::program::nothing_loaded::nothing_loaded() noexcept {}


fostlib::wliteral const fg::program::nothing_loaded::message() const {
    return L"No script has been loaded";
}
