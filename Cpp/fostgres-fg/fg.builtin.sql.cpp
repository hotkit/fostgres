/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/db.hpp>
#include <fostgres/fg/fg.hpp>
#include <fost/postgres>


fg::frame::builtin fg::lib::sql_file = [](fg::frame &stack,
                                          fg::json::const_iterator pos,
                                          fg::json::const_iterator end) {
    auto sql = fostlib::coerce<fostlib::utf8_string>(fostlib::utf::load_file(
            fostlib::coerce<boost::filesystem::path>(stack.resolve_string(
                    stack.argument("filename", pos, end)))));
    fostlib::pg::connection cnx(fostgres::connection(
            stack.lookup("pg.dsn"),
            fostlib::coerce<fostlib::nullable<fostlib::string>>(
                    stack.lookup("pg.zoneinfo"))));
    cnx.exec(sql);
    cnx.commit();
    return fostlib::json();
};


fg::frame::builtin fg::lib::sql_insert = [](fg::frame &stack,
                                            fg::json::const_iterator pos,
                                            fg::json::const_iterator end) {
    auto relation = stack.resolve_string(stack.argument("relation", pos, end));
    auto data = stack.argument("data", pos, end);
    fostlib::pg::connection cnx(fostgres::connection(
            stack.lookup("pg.dsn"),
            fostlib::coerce<fostlib::nullable<fostlib::string>>(
                    stack.lookup("pg.zoneinfo"))));
    cnx.insert(relation.shrink_to_fit(), data);
    cnx.commit();
    return fostlib::json();
};
