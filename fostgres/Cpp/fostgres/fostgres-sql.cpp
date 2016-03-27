/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/push_back>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>


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
                return fostgres::response(configuration, m.value(), req);
            }
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "No match found -- should be 404");
        }
    } c_fostgres_sql;


}

