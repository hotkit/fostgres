/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/push_back>
#include <fostgres/response.hpp>


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config, const fostlib::json &data
) {
    const bool pretty = fostlib::coerce<fostlib::nullable<bool>>(config["pretty"]).value(true);
    boost::shared_ptr<fostlib::mime> response(
            new fostlib::text_body(fostlib::json::unparse(data, pretty),
                fostlib::mime::mime_headers(), L"text/plain"));
    return std::make_pair(response, 200);
}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response(
    const fostlib::json &config,
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data
) {
    return response_csj(config, std::move(data));
}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response_json_csv(
    const fostlib::json &config,
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data
) {
    fostlib::json result;
    fostlib::insert(result, "@context", "http://www.kirit.com/fostgres/context/rows");
    { // Start with the header row
        fostlib::json header;
        for ( const auto &heading : data.first ) {
            fostlib::push_back(header, heading);
        }
        fostlib::insert(result, "columns", header);
    }
    { // Continue with rows
        fostlib::json rows;
        for ( const auto &record : data.second ) {
            fostlib::json row;
            for ( std::size_t index{0}; index < record.size(); ++index ) {
                fostlib::push_back(row, record[index]);
            }
            fostlib::push_back(rows, row);
        }
        fostlib::insert(result, "rows", rows);
    }
    return response(config, result);
}


namespace {
    struct csj_mime : public fostlib::mime {
        mutable bool done = false;
        mutable std::vector<fostlib::string> columns;
        mutable fostlib::pg::recordset rs;

        struct csj_iterator : public fostlib::mime::iterator_implementation {
            fostlib::pg::recordset rs;
            fostlib::pg::recordset::const_iterator iter, end;
            std::string current;
            bool sent_first = false;

            csj_iterator(std::vector<fostlib::string> &&columns, fostlib::pg::recordset &&r)
            : rs(std::move(r)), iter(rs.begin()), end(rs.end()) {
                current.reserve(64 * 1024);
                fostlib::stringstream ss;
                std::copy(columns.begin(), columns.end() - 1,
                    std::ostream_iterator<fostlib::string>(ss, ","));
                ss << columns.back() << std::endl;
                current = ss.str();
                if ( iter != end ) line();
            }

            void line() {
                while ( iter != end && current.length() < 48 * 1024 ) {
                    auto record = *iter;
                    current += fostlib::json::unparse(record[0], false).std_str();
                    for ( std::size_t index{1}; index < record.size(); ++index ) {
                        current += ',';
                        current += fostlib::json::unparse(record[index], false).std_str();
                    }
                    current += '\n';
                    ++iter;
                }
            }

            fostlib::const_memory_block operator () () {
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
        };

        csj_mime(std::vector<fostlib::string> &&cols, fostlib::pg::recordset &&rs)
        : mime(fostlib::mime::mime_headers(), "text/plain"),
            columns(std::move(cols)), rs(std::move(rs))
        {
        }

        std::unique_ptr<iterator_implementation> iterator() const {
            if ( done )
                throw fostlib::exceptions::not_implemented(__FUNCTION__);
            done = true;
            return std::unique_ptr<iterator_implementation>(
                new csj_iterator(std::move(columns), std::move(rs)));
        }

        bool boundary_is_ok(const fostlib::string &) const {
            throw fostlib::exceptions::not_implemented(__FUNCTION__);
        }
        std::ostream &print_on(std::ostream &) const {
            throw fostlib::exceptions::not_implemented(__FUNCTION__);
        }
    };
}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response_csj(
    const fostlib::json &config,
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data
) {
    return std::make_pair(
        boost::shared_ptr<fostlib::mime>(
            new csj_mime(std::move(data.first), std::move(data.second))),
        200);
}

