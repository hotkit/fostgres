/**
    Copyright 2019, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/
#include <pqxx/except>

#include <fostgres/response.hpp>

#include <chrono>
#include <thread>


namespace {


    const class fostgres_control_error : public fostlib::urlhandler::view {
      public:
        fostgres_control_error() : view("fostgres.control.retry") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            std::pair<boost::shared_ptr<fostlib::mime>, int> response{nullptr,
                                                                      0};
            std::size_t retries = 0u;
            while (not response.second) {
                try {
                    response = execute(config["try"], path, req, host);
                } catch (...) {
                    if (retries >= 3) {
                        response = execute(config["error"], path, req, host);
                    } else {
                        std::this_thread::sleep_for(
                                std::chrono::milliseconds{25});
                    }
                    ++retries;
                }
            }
            if (retries) {
                response.first->headers().add(
                        "Fostgres-pg-serialisation-retries",
                        fostlib::coerce<fostlib::string>(retries));
            }
            return response;
        }
    } c_fostgres_control_retry;


}
