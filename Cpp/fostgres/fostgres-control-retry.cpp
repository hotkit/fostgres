/**
    Copyright 2019, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/
#include <pqxx/except>

#include <fostgres/response.hpp>


namespace {


    const class fostgres_control_error : public fostlib::urlhandler::view {
      public:
        fostgres_control_error() : view("fostgres.control.retry") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            try {
                return execute(config["execute"], path, req, host);
            } catch (...) {
                throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__);
            }
        }
    } c_fostgres_control_retry;


}
