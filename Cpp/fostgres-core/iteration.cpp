/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log>
#include <fostgres/db.hpp>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {
    template<typename RS>
    std::vector<fostlib::string> columns(const RS &rs) {
        std::vector<fostlib::string> cols;
        std::size_t number{0};
        for ( const auto &c : rs.columns() ) {
            cols.push_back(c.value_or("un-named." + std::to_string(++number)));
        }
        return cols;
    }
}


std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> fostgres::column_names(
    fostlib::pg::recordset && rs
) {
    return std::make_pair(columns(rs), std::move(rs));
}


/*
 * fostgres::json_recordset
 */


fostgres::json_recordset::json_recordset(std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> rs)
: names(std::move(rs.first)), rs(std::move(rs.second)) {
}


fostgres::json_recordset::const_iterator fostgres::json_recordset::begin() const {
    return {names, rs.begin()};
}


fostgres::json_recordset::const_iterator fostgres::json_recordset::end() const {
    return {names, rs.end()};
}


/*
 * fostgres::json_recordset::const_iterator
 */


fostgres::json_recordset::const_iterator::const_iterator(
    const std::vector<fostlib::string> &n,
    fostlib::pg::recordset::const_iterator p)
: names(&n), pos(std::move(p)) {
}


fostgres::json_recordset::const_iterator &fostgres::json_recordset::const_iterator::operator ++ () {
    ++pos;
    return *this;
}


fostlib::json::object_t &fostgres::json_recordset::const_iterator::operator * () {
    if ( not names ) {
        throw fostlib::exceptions::null(
            "Can't dereference an empty json_recordset::const_iterator");
    }
    const auto &row = *pos;
    for ( std::size_t index{}; index < names->size(); ++index ) {
        object[(*names)[index]] = row[index];
    }
    return object;
}

