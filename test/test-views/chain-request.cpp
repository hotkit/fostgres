/**
    Copyright 2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/http>
#include <fost/ua/exceptions.hpp>
#include <fost/urlhandler>


namespace {


    const class chain_request : public fostlib::urlhandler::view {
      public:
        chain_request() : view("test.chain.request") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &config,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {

            auto const body_str = fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data()));
            auto const body = fostlib::json::parse(body_str);

            fostlib::mime::mime_headers headers;
            fostlib::string url = "http://127.0.0.1/test/"
                    + fostlib::coerce<fostlib::string>(body["id"]);
            boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                    fostlib::json::unparse(fostlib::json{}, true), headers,
                    "application/json"));

            fostlib::json result;
            try {
                if (fostlib::coerce<fostlib::string>(body["method"]) == "GET") {
                    result = fostlib::ua::get_json(fostlib::url{url}, headers);
                } else {
                    result = fostlib::ua::post_json(
                            fostlib::url{url}, fostlib::json{}, headers);
                }
            } catch (fostlib::ua::unauthorized &e) {
                return std::make_pair(response, 401);
            } catch (fostlib::ua::forbidden &e) {
                return std::make_pair(response, 403);
            } catch (fostlib::ua::resource_not_found &e) {
                return std::make_pair(response, 404);
            } catch (fostlib::ua::http_error &e) {
                auto status = fostlib::coerce<int>(e.data()["status-code"]);
                return std::make_pair(response, status);
            }
            response = boost::shared_ptr<fostlib::mime>(new fostlib::text_body(
                    fostlib::json::unparse(result, true), headers,
                    "application/json"));
            return std::make_pair(response, 200);
        }

    } c_chain_request;


}
