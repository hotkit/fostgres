/**
    Copyright 2016-2018, Felspar Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include "updater.hpp"

#include <fostgres/fostgres.hpp>

#include <fost/insert>
#include <fost/log>


namespace {


    std::pair<boost::shared_ptr<fostlib::mime>, int>
            get(fostlib::pg::connection &cnx,
                const fostlib::json &config,
                const fostgres::match &m,
                fostlib::http::server::request &req) {
        return fostgres::response_object(
                select_data(cnx, m.configuration["GET"], m, req), config);
    }

    fostlib::json
            calc_keys(const fostgres::match &m, const fostlib::json &config) {
        fostlib::json keys;
        for (std::size_t index{0}; index != config.size(); ++index) {
            auto key = fostlib::coerce<fostlib::string>(config[index]);
            fostlib::insert(keys, key, m.arguments[index]);
        }
        return keys;
    }
    fostlib::json
            calc_values(const fostlib::json &body, const fostlib::json config) {
        fostlib::json values;
        for (auto value : config) {
            auto vname = fostlib::coerce<f5::u8view>(value);
            if (body.has_key(vname)) {
                fostlib::insert(values, vname, body[vname]);
            }
        }
        return values;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int> proc_put(
            fostlib::pg::connection &cnx,
            const fostlib::json &config,
            const fostgres::match &m,
            fostlib::http::server::request &req,
            const fostlib::json &put_config,
            const fostlib::json &body) {
        auto error = fostgres::schema_check(
                cnx, config, m, req, put_config, body, fostlib::jcursor{});
        if (error.first || error.second) return error;
        if (put_config.has_key("columns")) {
            return fostgres::updater{put_config, cnx, m, req}.upsert(body).first;
        } else {
            fostlib::string relation =
                    fostlib::coerce<fostlib::string>(put_config["table"]);
            fostlib::json keys(calc_keys(m, put_config["keys"]));
            fostlib::json values(calc_values(body, put_config["attributes"]));
            cnx.upsert(relation.c_str(), keys, values);
        }
        return std::make_pair(nullptr, 0);
    }
    std::pair<boost::shared_ptr<fostlib::mime>, int>
            put(fostlib::pg::connection &cnx,
                const fostlib::json &config,
                const fostgres::match &m,
                fostlib::http::server::request &req) {
        fostlib::json body{fostlib::json::parse(fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        auto put_config = m.configuration["PUT"];
        std::pair<boost::shared_ptr<fostlib::mime>, int> returning;
        if (put_config.isobject()) {
            returning = proc_put(cnx, config, m, req, put_config, body);
        } else if (put_config.isarray()) {
            for (const auto &cfg : put_config) {
                returning = proc_put(cnx, config, m, req, cfg, body);
                if (returning.second >= 400) break;
            }
        }
        cnx.commit();
        if (returning.first) {
            return returning;
        } else {
            return get(cnx, config, m, req);
        }
    }

    std::pair<boost::shared_ptr<fostlib::mime>, int> proc_post(
            fostlib::pg::connection &cnx,
            const fostlib::json &config,
            const fostgres::match &m,
            fostlib::http::server::request &req,
            const fostlib::json &post_config,
            const fostlib::json &body) {
        fostlib::string relation =
                fostlib::coerce<fostlib::string>(post_config["table"]);
        fostlib::json col_config = post_config["columns"];
        fostlib::json values;
        for (auto col_def = col_config.begin(); col_def != col_config.end();
             ++col_def) {
            const auto name = fostlib::coerce<fostlib::string>(col_def.key());
            const auto data =
                    fostgres::datum(name, *col_def, m.arguments, body, req);
            if (data) { fostlib::insert(values, name, data.value()); }
        }
        const fostlib::json &ret_cols = post_config["returning"];
        std::vector<fostlib::string> returning;
        std::transform(
                ret_cols.begin(), ret_cols.end(), std::back_inserter(returning),
                [](const auto &s) {
                    return fostlib::coerce<fostlib::string>(s);
                });
        if (not returning.size()) { returning.emplace_back("*"); }
        auto result = fostgres::column_names(
                cnx.insert(relation.c_str(), values, returning));
        return fostgres::response_object(std::move(result), config);
    }

    std::pair<boost::shared_ptr<fostlib::mime>, int>
            post(fostlib::pg::connection &cnx,
                 const fostlib::json &config,
                 const fostgres::match &m,
                 fostlib::http::server::request &req) {
        fostlib::json body{fostlib::json::parse(fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        std::pair<boost::shared_ptr<fostlib::mime>, int> returning;
        auto const post_config = m.configuration["POST"];
        if (post_config.isobject()) {
            returning = proc_post(cnx, config, m, req, post_config, body);
        } else if (post_config.isarray()) {
            for (const auto &cfg : post_config) {
                returning = proc_post(cnx, config, m, req, cfg, body);
            }
        }
        cnx.commit();
        return returning;
    }

    std::pair<boost::shared_ptr<fostlib::mime>, int>
            patch(fostlib::pg::connection &cnx,
                  const fostlib::json &config,
                  const fostgres::match &m,
                  fostlib::http::server::request &req) {
        fostlib::json body{fostlib::json::parse(fostlib::coerce<fostlib::string>(
                fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        auto error = fostgres::schema_check(
                cnx, config, m, req, m.configuration["PATCH"], body,
                fostlib::jcursor{});
        if (error.first || error.second) return error;

        fostlib::string relation = fostlib::coerce<fostlib::string>(
                m.configuration["PATCH"]["table"]);
        if (m.configuration["PATCH"].has_key("columns")) {
            fostgres::updater{m.configuration["PATCH"], cnx, m, req}.update(
                    body);
            cnx.commit();
        } else {
            fostlib::log::warning(fostgres::c_fostgres)(
                    "",
                    "PATCH configuration with 'keys' and 'attributes' is "
                    "deprecated. Use 'columns'")(
                    "configuration", m.configuration["PATCH"]);
            fostlib::json keys(calc_keys(m, m.configuration["PATCH"]["keys"]));
            fostlib::json values(
                    calc_values(body, m.configuration["PATCH"]["attributes"]));
            cnx.update(relation.c_str(), keys, values).commit();
        }
        return get(cnx, config, m, req);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>
            del(fostlib::pg::connection &cnx,
                const fostlib::json &config,
                const fostgres::match &m,
                fostlib::http::server::request &req) {
        auto get_result = get(cnx, config, m, req);
        auto sql = fostlib::coerce<fostlib::string>(m.configuration["DELETE"]);
        auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(sql));
        sp.exec(m.arguments);
        cnx.commit();
        return get_result;
        ;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int> response_object(
            const fostlib::json &config,
            const fostgres::match &m,
            fostlib::http::server::request &req) {
        fostlib::pg::connection cnx(fostgres::connection(config, req));
        if (req.method() == "GET" or req.method() == "HEAD") {
            return get(cnx, config, m, req);
        } else if (req.method() == "PATCH") {
            return patch(cnx, config, m, req);
        } else if (req.method() == "POST") {
            return post(cnx, config, m, req);
        } else if (req.method() == "PUT") {
            return put(cnx, config, m, req);
        } else if (req.method() == "DELETE") {
            return del(cnx, config, m, req);
        } else {
            throw fostlib::exceptions::not_implemented(
                    __FUNCTION__, "Invalid HTTP method -- should return 405");
        }
    }


    const fostgres::responder c_object("object", response_object);


}


std::pair<boost::shared_ptr<fostlib::mime>, int> fostgres::response_object(
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data,
        const fostlib::json &config) {
    const bool pretty =
            fostlib::coerce<fostlib::nullable<bool>>(config["pretty"])
                    .value_or(true);
    auto row = data.second.begin();
    if (row == data.second.end()) { // TODO:Use data.second.empty() instead?
        fostlib::json result;
        insert(result, "error", "Not found");
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
                fostlib::json::unparse(result, pretty),
                fostlib::mime::mime_headers(), L"application/json"));
        return std::make_pair(response, 404);
    }
    fostlib::json result;
    auto record = *row;
    for (std::size_t index{0}; index < record.size(); ++index) {
        if (data.first[index].endswith("__tableoid")) continue;
        const auto parts = fostlib::split(data.first[index], "__");
        fostlib::jcursor pos;
        for (const auto &p : parts) pos /= p;
        fostlib::insert(result, pos, record[index]);
    }
    if (++row != data.second.end()) {
        // TODO Return proper error
        throw fostlib::exceptions::not_implemented(
                __func__, "Too many rows returned");
    }
    boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(
            fostlib::json::unparse(result, pretty),
            fostlib::mime::mime_headers(), L"application/json"));
    return std::make_pair(response, 200);
}
