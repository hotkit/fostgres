/**
    Copyright 2019, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "nonce.hpp"
#include <fost/log>
#include <fost/urlhandler>
#include <fostgres/fostgres.hpp>

#include "nonce.hpp"


namespace {


    fostlib::module const c_rqlog{fostgres::c_fostgres, "request-logger"};


    class capture_copy {
        bool pass_on;

      public:
        using result_type = std::vector<fostlib::json> const &;
        std::vector<fostlib::json> messages;

        capture_copy(bool pass_on = true) : pass_on{pass_on} {}

        bool operator()(const fostlib::log::message &m) {
            messages.push_back(fostlib::coerce<fostlib::json>(m));
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
            fostlib::log::scoped_sink<capture_copy> logs;
            auto const rqid = rqlog::reference();
            req.headers().add("__request_id", rqid);
            {
                std::pair<boost::shared_ptr<fostlib::mime>, int> response;
                auto logger = fostlib::log::debug(c_rqlog);
                logger("request", "id", rqid);
                try {
                    response = view::execute(config["view"], path, req, host);
                } catch (...) { throw; }
                return response;
            }
        }
    } c_request_logger;


}
