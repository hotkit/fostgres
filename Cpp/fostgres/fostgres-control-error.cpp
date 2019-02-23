/**
    Copyright 2019, Felspar Co Ltd. <http://support.felspar.com/>

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
                return execute(config["execute"], path, req, host);
            } catch (pqxx::serialization_failure const &e) {
                if (config.has_key("40001")) {
                    return execute(config["40001"], path, req, host);
                } else {
                    return execute(config[""], path, req, host);
                }
            } catch (pqxx::sql_error const &e) {
                if (config.has_key(e.sqlstate().c_str())) {
                    return execute(
                            config[e.sqlstate().c_str()], path, req, host);
                } else {
                    return execute(config[""], path, req, host);
                }
            } catch (pqxx::pqxx_exception const &e) {
                return execute(config[""], path, req, host);
            }
        }

    } c_fostgres_control_error;


}
