/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/push_back>
#include <fostgres/matcher.hpp>
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
            auto m = fostgres::matcher(configuration["sql"], path);
            if ( not m.isnull() ) {
                if ( m.value().arguments.size() ) {
                    return fostgres::response(m.value().configuration, fostgres::sql(
                        fostlib::coerce<fostlib::string>(configuration["host"]),
                        fostlib::coerce<fostlib::string>(configuration["database"]),
                        fostlib::coerce<fostlib::string>(m.value().configuration["GET"]),
                        m.value().arguments));
                } else {
                    return fostgres::response(m.value().configuration, fostgres::sql(
                        fostlib::coerce<fostlib::string>(configuration["host"]),
                        fostlib::coerce<fostlib::string>(configuration["database"]),
                        fostlib::coerce<fostlib::string>(m.value().configuration["GET"])));
                }
            }
            throw fostlib::exceptions::not_implemented(__FUNCTION__);
        }
    } c_fostgres_sql;


}

