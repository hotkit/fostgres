/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/fg/mime.hpp>
#include <fostgres/fg/fg.testserver.hpp>
#include <fost/exception/parse_error.hpp>
#include <fost/insert>


namespace {


    std::pair<boost::shared_ptr<fostlib::mime>, int>
            perform(fg::frame &stack,
                    fostlib::http::server::request &req,
                    const fostlib::string &path,
                    int expected_status) {
        if (path.empty()) {
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "Requested path is empty");
        }
        fostlib::host host("localhost");
        /**
         * By processing the cookies first and putting them in the request
         * headers first the `testserver.headers` can override the entire
         * cookie header.
         */
        auto cookies = stack.lookup("testserver.cookies");
        for (auto [cname, cvalue] : cookies.object()) {
            if (req.headers().exists("Cookie")) {
                req.headers().set(
                        "Cookie",
                        req.headers()["Cookie"].value() + "; " + cname + "="
                                + fostlib::coerce<fostlib::string>(cvalue));
            } else {
                req.headers().set(
                        "Cookie",
                        cname + "=" + fostlib::coerce<fostlib::string>(cvalue));
            }
        }
        auto headers = stack.lookup("testserver.headers");
        for (auto h(headers.begin()); h != headers.end(); ++h) {
            req.headers().set(
                    fostlib::coerce<fostlib::string>(h.key()),
                    fostlib::coerce<fostlib::string>(*h));
        }
        try {
            auto view_result = fostlib::urlhandler::view::execute(
                    fg::json("fg.test"), path.substr(1), req, host);
            if (req.method() == "HEAD") {
                // HEAD returns empty body so we will not use the view result response body
                auto response = boost::make_shared<fostlib::empty_mime>();
                return {response, view_result.second};
            }
            return view_result;
        } catch (fostlib::exceptions::parse_error &e) {
            if (expected_status == 400) {
                auto response = boost::make_shared<fostlib::text_body>(
                        fostlib::string{"<html><head><title>Parse "
                                        "error</title></head><body><h1>Parse "
                                        "error</h1></body></html>"},
                        fostlib::mime::mime_headers(), "text/html");
                return {response, 400};
            } else {
                fostlib::insert(e.data(), "request", "headers", req.headers());
                throw;
            }
        } catch (fostlib::exceptions::not_implemented &e) {
            if (expected_status == 501) {
                return fostlib::urlhandler::view::execute(
                        fostlib::json{"fost.response.501"}, path, req, host);
            } else {
                fostlib::insert(e.data(), "request", "headers", req.headers());
                throw;
            }
        }
    }


    auto
            nobody(f5::lstring method,
                   fg::frame &stack,
                   const fostlib::string &path,
                   int expected_status) {
        fostlib::http::server::request request(
                method, fostlib::coerce<fostlib::url::filepath_string>(path));
        return perform(stack, request, path, expected_status);
    }
    auto withbody(
            f5::lstring method,
            fg::frame &stack,
            const fostlib::string &path,
            std::unique_ptr<fostlib::binary_body> body,
            int expected_status) {
        fostlib::http::server::request request(
                method, fostlib::coerce<fostlib::url::filepath_string>(path),
                std::move(body));
        return perform(stack, request, path, expected_status);
    }


}


/**
    ## `fg::testserver`
*/


fg::testserver::testserver(const frame &stack, const fostlib::json &vn)
: viewname(vn),
  host_config(
          "fg.testserver.cpp",
          fostlib::urlhandler::c_hosts,
          []() {
              fg::json hosts;
              fostlib::insert(hosts, "", "fg.test");
              return hosts;
          }()),
  view_config("fg.testserver.cpp", "webserver", "views/fg.test", [&]() {
      fg::json views;
      fostlib::insert(views, "view", "fost.middleware.request");
      if (vn.isobject()) {
          fostlib::insert(views, "configuration", vn);
      } else {
          fostlib::insert(views, "configuration", "view", vn);
      }
      fostlib::insert(
              views, "configuration", "headers", "__pgdsn",
              stack.lookup("pg.dsn"));
      auto zi = stack.lookup("pg.zoneinfo");
      if (not zi.isnull()) {
          fostlib::insert(
                  views, "configuration", "headers", "__pgzoneinfo",
                  fostlib::coerce<fostlib::string>(zi));
      }
      return views;
  }()) {}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::get(
        frame &stack, const fostlib::string &path, int expected_status) {
    return nobody("GET", stack, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::head(
        frame &stack, const fostlib::string &path, int expected_status) {
    return nobody("HEAD", stack, path, expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::patch(
        frame &stack,
        const fostlib::string &path,
        std::unique_ptr<fostlib::binary_body> body,
        int expected_status) {
    return withbody("PATCH", stack, path, std::move(body), expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::put(
        frame &stack,
        const fostlib::string &path,
        std::unique_ptr<fostlib::binary_body> body,
        int expected_status) {
    return withbody("PUT", stack, path, std::move(body), expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::post(
        frame &stack,
        const fostlib::string &path,
        std::unique_ptr<fostlib::binary_body> body,
        int expected_status) {
    return withbody("POST", stack, path, std::move(body), expected_status);
}


std::pair<boost::shared_ptr<fostlib::mime>, int> fg::testserver::del(
        frame &stack, const fostlib::string &path, int expected_status) {
    return nobody("DELETE", stack, path, expected_status);
}


/// ## `mismatched_status_code`


fg::mismatched_status_code::mismatched_status_code(int64_t e, int a)
: expected{e}, actual{a} {}


const wchar_t *const fg::mismatched_status_code::message() const noexcept {
    return L"Mismatched HTTP status codes";
}
