/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/push_back>

#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include "precondition.hpp"

namespace {


    const class fostgres_sql : public fostlib::urlhandler::view {
      public:
        fostgres_sql() : view("fostgres.sql") {}

        std::pair<boost::shared_ptr<fostlib::mime>, int> operator()(
                const fostlib::json &configuration,
                const fostlib::string &path,
                fostlib::http::server::request &req,
                const fostlib::host &host) const {
            auto m = fostgres::matcher(configuration["sql"], path);
            if (m) {
                if (m.value().configuration.has_key("precondition")){
                    fostlib::json precondition_config = m.value().configuration["precondition"];
                    fostlib::json precondition_predicates;
                    if (precondition_config.isobject()){
                        precondition_predicates = precondition_config["check"];
                    } else {
                        precondition_predicates = precondition_config;
                    }
                    auto stack = fostgres::preconditions(req, m.value().arguments);
                    const auto res = fsigma::call(stack, precondition_predicates);
                    if (res.isnull()){
                        // precondition predicate result is Falsy
                        if (precondition_config.isobject() && precondition_config.has_key("failed")){
                            return execute(precondition_config["failed"], path, req, host);
                        }
                        /// Fallback to 403
                        fostlib::json config;
                        fostlib::insert(config, "view", "fost.response.403");
                        return execute(config, path, req, host);
                    }
                }
                try {
                    return fostgres::response(configuration, m.value(), req);
                } catch (fostlib::exceptions::exception &e) {
                    fostlib::insert(
                            e.data(), "view", "matched",
                            m.value().configuration);
                    throw;
                }
            }
            throw fostlib::exceptions::not_implemented(
                    __PRETTY_FUNCTION__, "No match found -- should be 404");
        }
    } c_fostgres_sql;


}
