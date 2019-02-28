/**
    Copyright 2019, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "nonce.hpp"
#include <fost/datetime>
#include <fost/log>
#include <fost/timer>
#include <fost/urlhandler>
#include <fostgres/fostgres.hpp>
#include <fostgres/sql.hpp>

#include "nonce.hpp"


namespace {


    fostlib::module const c_rqlog{fostgres::c_fostgres, "request-logger"};


    class capture_copy {
        bool pass_on;
        fostlib::json messages;

      public:
        using result_type = fostlib::json;

        capture_copy(bool pass_on = true) : pass_on{pass_on} {}

        bool operator()(const fostlib::log::message &m) {
            fostlib::push_back(messages, fostlib::coerce<fostlib::json>(m));
            return pass_on;
        }

        result_type operator()() const { return messages; }
    };


    const class request_logger : public fostlib::urlhandler::view {
      public:
        request_logger() : view("fostgres.request-logging") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            fostlib::timer time;
            fostlib::log::scoped_sink<capture_copy> logs;
            auto const rqid = rqlog::reference();
            fostlib::json row;
            fostlib::insert(row, "id", rqid);
            fostlib::insert(row, "request_path", path);
            fostlib::insert(row, "request_headers", req.headers());
            req.headers().add("Fostgres-Request-ID", rqid);
            std::pair<boost::shared_ptr<fostlib::mime>, int> response;
            std::exception_ptr exception;
            {
                auto logger = fostlib::log::debug(c_rqlog);
                logger("request", "id", rqid);
                try {
                    response = view::execute(config["view"], path, req, host);
                    response.first->headers().add("Fostgres-Request-ID", rqid);
                    fostlib::insert(
                            row, "response_headers", response.first->headers());
                    fostlib::insert(row, "status", response.second);
                } catch (std::exception const &e) {
                    exception = std::current_exception();
                    fostlib::insert(row, "exception", e.what());
                } catch (...) {
                    exception = std::current_exception();
                    fostlib::insert(row, "exception", "**unknown**");
                }
            }
            fostlib::log::flush();
            fostlib::insert(
                    row, "messages", fostlib::json::unparse(logs(), false));
            fostlib::insert(row, "started", time.started());
            fostlib::insert(row, "duration", time.seconds());
            try {
                fostlib::pg::connection cnx(fostgres::connection(config, req));
                cnx.insert("request_log", row);
                cnx.commit();
            } catch (...) {
                fostlib::log::error(c_rqlog)(
                        "",
                        "Error saving log message to database")("data", row);
            }
            if (exception) { std::rethrow_exception(exception); }
            return response;
        }
    } c_request_logger;


}
