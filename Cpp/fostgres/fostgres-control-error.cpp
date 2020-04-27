/**
    Copyright 2019-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */
#include <pqxx/except>

#include <fost/insert>
#include <fost/push_back>

#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>


namespace {


    const class fostgres_control_error : public fostlib::urlhandler::view {
      public:
        fostgres_control_error() : view("fostgres.control.pg-error") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            try {
                if (not config.has_key("execute")) {
                    throw fostlib::exceptions::not_implemented{
                            __PRETTY_FUNCTION__, "Missing execute key"};
                }
                return execute(config["execute"], path, req, host);
            } catch (pqxx::sql_error const &e) {
                f5::u8string sqlstate{e.sqlstate()};
                if (config.has_key(sqlstate)) {
                    return execute(config[sqlstate], path, req, host);
                } else {
                    return execute(config[""], path, req, host);
                }
            } catch (pqxx::failure const &) {
                return execute(config[""], path, req, host);
            }
        }

    } c_fostgres_control_error;


}
