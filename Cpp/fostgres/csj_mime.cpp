/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fostgres/csj_mime.hpp>
#include <fostgres/fostgres.hpp>


namespace {


    void csv_string(std::string &into, const fostlib::string &str) {
        if ( str.find_first_of("\"\n,") != fostlib::string::npos ) {
            into += '"';
            for ( auto ch : str.std_str() ) {
                switch ( ch ) {
                case '"':
                    into += ch; // Double up this one
                default:
                    into += ch;
                }
            }
            into += '"';
        } else {
            into += str.std_str();
        }
    }


    fostlib::nliteral mime_type(const fostlib::string &accept) {
        const auto csj_pos = accept.find("application/csj");
        const auto csv_pos = accept.find("application/csv");
        if ( csj_pos < csv_pos ) {
            return "application/csj";
        } else if ( csv_pos < csj_pos ) {
            return "application/csv";
        } else {
            return "text/plain";
        }
    }


}


/*
 * fostgres::csj_mime
 */


fostgres::csj_mime::csj_mime(
    const fostlib::string &accept,
    std::vector<fostlib::string> &&cols,
    fostlib::pg::recordset &&rs)
: mime(fostlib::mime::mime_headers(), mime_type(accept)),
    format(output::csj), columns(std::move(cols)), rs(std::move(rs))
{
    if ( headers()["Content-Type"].value() == "application/csv" ) {
        format = output::csv;
    }
}


auto fostgres::csj_mime::iterator() const -> std::unique_ptr<iterator_implementation> {
    if ( done ) {
        throw fostlib::exceptions::not_implemented(__func__,
            "The data can only be iterated over once");
    }
    done = true;
    return std::unique_ptr<iterator_implementation>(
        new csj_iterator(format, std::move(columns), std::move(rs)));
}


/*
 * fostgres::csj_mime::csj_iterator
 */


fostgres::csj_mime::csj_iterator::csj_iterator(
    csj_mime::output format,
    std::vector<fostlib::string> &&columns,
    fostlib::pg::recordset &&r)
: format(format), rs(std::move(r)), iter(rs.begin()), end(rs.end()) {
    current.reserve(64 * 1024);
    for ( std::size_t index{0}; index < columns.size(); ++index ) {
        if ( index ) current += ',';
        switch ( format ) {
        case csj_mime::output::csj:
            fostlib::json::unparse(current, columns[index]);
            break;
        case csj_mime::output::csv:
            csv_string(current, columns[index]);
            break;
        }
    }
    current += '\n';
    if ( iter != end ) line();
}


void fostgres::csj_mime::csj_iterator::line() {
    while ( iter != end && current.length() < 48 * 1024 ) {
        auto record = *iter;
        for ( std::size_t index{0}; index < record.size(); ++index ) {
            if ( index ) current += ',';
            switch ( format ) {
            case csj_mime::output::csj:
                fostlib::json::unparse(current, record[index], false);
                break;
            case csj_mime::output::csv:
                if ( record[index].isnull() ) {
                    // Do nothing -- null is an empty entry
                } else if ( not record[index].get<fostlib::string>() ) {
                    csv_string(current, fostlib::json::unparse(record[index], false));
                } else {
                    csv_string(current, fostlib::coerce<fostlib::string>(record[index]));
                }
                break;
            }
        }
        current += '\n';
        ++iter;
    }
}


fostlib::const_memory_block fostgres::csj_mime::csj_iterator::operator () () {
    if ( !sent_first ) {
        sent_first = true;
        return fostlib::const_memory_block(
            current.c_str(), current.c_str() + current.length());
    } else if ( iter == end ) {
        return fostlib::const_memory_block();
    } else {
        current.clear();
        line();
        return fostlib::const_memory_block(
            current.c_str(), current.c_str() + current.length());
    }
}

