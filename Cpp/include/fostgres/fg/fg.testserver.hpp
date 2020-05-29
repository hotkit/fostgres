/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fostgres/fg/fg.hpp>
#include <fost/urlhandler>


namespace fg {


    /// Can be used to check the result of running a request against a URL
    class testserver {
        fostlib::json viewname;
        fostlib::setting<json> host_config, view_config;

      public:
        testserver(const fg::frame &, const fostlib::json &viewname);

        /// Return the result of the GET request
        std::pair<boost::shared_ptr<fostlib::mime>, int> get(
                frame &stack, const fostlib::string &path, int expected_status);
        /// Return the result of the HEAD request
        std::pair<boost::shared_ptr<fostlib::mime>, int> head(
                frame &stack, const fostlib::string &path, int expected_status);
        /// Return the result of the PATCH request
        std::pair<boost::shared_ptr<fostlib::mime>, int>
                patch(frame &stack,
                      const fostlib::string &path,
                      std::unique_ptr<fostlib::binary_body> body,
                      int expected_status);
        /// Return the result of the PUT request
        std::pair<boost::shared_ptr<fostlib::mime>, int>
                put(frame &stack,
                    const fostlib::string &path,
                    std::unique_ptr<fostlib::binary_body> body,
                    int expected_status);
        /// Return the result of the POST request
        std::pair<boost::shared_ptr<fostlib::mime>, int>
                post(frame &stack,
                     const fostlib::string &path,
                     std::unique_ptr<fostlib::binary_body> body,
                     int expected_status);
        /// Return the result of the DELETE request
        std::pair<boost::shared_ptr<fostlib::mime>, int> del(
                frame &stack, const fostlib::string &path, int expected_status);
    };


    /// Exception thrown when the HTTP status code in the test doesn't match
    /// the one that was returned
    class mismatched_status_code : public fostlib::exceptions::exception {
      public:
        mismatched_status_code(int64_t expected, int actual);

        int64_t const expected;
        int const actual;

      protected:
        const wchar_t *const message() const noexcept override;
    };


}
