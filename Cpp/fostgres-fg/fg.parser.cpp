/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/fg/fg.hpp>
#include <fost/parse/json.hpp>
#include <fost/push_back>


namespace {
    const auto space_p = boost::spirit::chlit<wchar_t>(L' ');
    const auto newline_p = boost::spirit::chlit<wchar_t>(L'\n');
    const auto open_p = boost::spirit::chlit<wchar_t>(L'(');
    const auto close_p = boost::spirit::chlit<wchar_t>(L')');
    const auto string_p = +(boost::spirit::anychar_p - open_p - close_p - space_p - newline_p);
    const auto comment_p = *space_p >> boost::spirit::chlit<wchar_t>(L'#')
        >> *(boost::spirit::anychar_p - newline_p);
    const fostlib::json_string_parser json_string_p;
    const fostlib::json_embedded_parser json_ep;
    const fostlib::json_parser json_p;

    struct sexpr_closure : boost::spirit::closure<sexpr_closure, fg::json> {
        member1 value;
    };

    const struct sexpr_parser : public boost::spirit::grammar<
        sexpr_parser, sexpr_closure::context_t
    > {
        sexpr_parser() {}

        template< typename scanner_t >
        struct definition {
            definition(sexpr_parser const& self) {
                top = sexpr[self.value = phoenix::arg1];

                sexpr =
                        expr[fostlib::parsers::push_back(sexpr.value, phoenix::arg1)]
                        >> *(*space_p
                            >> expr[fostlib::parsers::push_back(sexpr.value, phoenix::arg1)]);

                expr =
                    json_ep[expr.value = phoenix::arg1]
                    | embedded[expr.value = phoenix::arg1]
                    | string_p[expr.value = phoenix::construct_<fostlib::string>(phoenix::arg1, phoenix::arg2)];

                embedded =
                    open_p
                    >> *space_p
                    >> sexpr[embedded.value = phoenix::arg1]
                    >> *space_p
                    >> close_p;
            }
            boost::spirit::rule<scanner_t, sexpr_closure::context_t> sexpr, expr, embedded;
            boost::spirit::rule<scanner_t> top;

            boost::spirit::rule<scanner_t> const &start() const { return top; }
        };
    } sexpr_p;
}


fostlib::json fg::parse(const boost::filesystem::path &filename) {
    fostlib::string code(fostlib::utf::load_file(filename));
    fostlib::json script;
    fostlib::push_back(script, "progn");

    fostlib::parser_lock lock;
    auto result = fostlib::parse(lock, code.c_str(), *(
        newline_p
        | comment_p
        | sexpr_p
            [([&script](auto j) {
                fostlib::push_back(script, j);
            })]
        ));
    if ( not result.full ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "Parse error", script);
    }
    return script;
}

