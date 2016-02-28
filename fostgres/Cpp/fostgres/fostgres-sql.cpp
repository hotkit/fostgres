/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/push_back>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {


    const class fostgres_sql : public fostlib::urlhandler::view {
    public:
        fostgres_sql()
        : view("fostgres.sql") {
        }

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator () (
            const fostlib::json &configuration, const fostlib::string &path,
            fostlib::http::server::request &req,
            const fostlib::host &
        ) const {
            fostlib::json::array_t rows;
            fostlib::push_back(rows, path);
            for ( const auto conf : configuration["sql"] ) {
                if ( conf.has_key("GET") ) {
                    auto data = fostgres::sql(
                        fostlib::coerce<fostlib::string>(configuration["host"]),
                        fostlib::coerce<fostlib::string>(configuration["database"]),
                        fostlib::coerce<fostlib::string>(conf["GET"]));
                    return fostgres::response(data);
                }
                fostlib::push_back(rows, conf);
            }
            return fostgres::response(rows);
        }
    } c_fostgres_sql;


}

