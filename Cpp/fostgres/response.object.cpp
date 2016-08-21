/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/insert>
#include <fost/log>
#include <fostgres/fostgres.hpp>
#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace {


    std::pair<boost::shared_ptr<fostlib::mime>, int>  get(
        fostlib::pg::connection &cnx,
        std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> &&data,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json result;
        auto row = data.second.begin();
        if ( row == data.second.end() ) {
            // TODO Return proper 404
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "No rows returned");
        }
        auto record = *row;
        for ( std::size_t index{0}; index < record.size(); ++index ) {
            if ( data.first[index].endswith("__tableoid") )
                continue;
            const auto parts = fostlib::split(data.first[index], "__");
            fostlib::jcursor pos;
            for ( const auto &p : parts ) pos /= p;
            fostlib::insert(result, pos, record[index]);
        }
        if ( ++row != data.second.end() ) {
            // TODO Return proper error
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "Too many rows returned");
        }
        const bool pretty = fostlib::coerce<fostlib::nullable<bool>>(config["pretty"]).value(true);
        boost::shared_ptr<fostlib::mime> response(
                new fostlib::text_body(fostlib::json::unparse(result, pretty),
                    fostlib::mime::mime_headers(), L"application/json"));
        return std::make_pair(response, 200);
    }
    std::pair<boost::shared_ptr<fostlib::mime>, int>  get(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        auto sql = m.configuration["GET"];
        if ( sql.isobject() ) {
            std::vector<fostlib::json> arguments;
            for ( const auto &arg : sql["arguments"] ) {
                try {
                    arguments.push_back(fostgres::datum(
                        arg, m.arguments, fostlib::json(), req).value(fostlib::json()));
                } catch ( fostlib::exceptions::exception &e ) {
                    insert(e.data(), "datum", arg);
                    throw;
                }
            }
            return get(cnx,
                fostgres::sql(cnx, fostlib::coerce<fostlib::string>(sql["command"]), arguments),
                config, m, req);
        } else {
            return get(cnx,
                fostgres::sql(cnx, fostlib::coerce<fostlib::string>(sql), m.arguments),
                config, m, req);
        }
    }

    fostlib::json calc_keys(const fostgres::match &m, const fostlib::json &config) {
        fostlib::json keys;
        for ( std::size_t index{0}; index != config.size(); ++index ) {
            auto key = fostlib::coerce<fostlib::string>(config[index]);
            fostlib::insert(keys, key, m.arguments[index]);
        }
        return keys;
    }
    fostlib::json calc_values(const fostlib::json &body, const fostlib::json config) {
        fostlib::json values;
        for ( auto value : config ) {
            auto vname = value.get<fostlib::string>().value();
            if ( body.has_key(vname) ) {
                fostlib::insert(values, vname, body[vname]);
            }
        }
        return values;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  put(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json body{
            fostlib::json::parse(
                fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        fostlib::string relation = fostlib::coerce<fostlib::string>(m.configuration["PUT"]["table"]);
        if ( m.configuration["PUT"].has_key("columns") ) {
            fostlib::json keys, values, col_config = m.configuration["PUT"]["columns"];
            for ( auto col_def = col_config.begin(); col_def != col_config.end(); ++col_def ) {
                auto key = fostlib::coerce<fostlib::string>(col_def.key());
                auto data = fostgres::datum(key, *col_def, m.arguments, body, req);
                if ( (*col_def)["key"].get(false) ) {
                    // Key column
                    if ( not data.isnull() ) {
                        fostlib::insert(keys, key, data.value());
                    } else {
                        throw fostlib::exceptions::not_implemented(__func__,
                            "Key column doesn't have a value", key);
                    }
                } else if ( not data.isnull() ) {
                    // Value column
                    fostlib::insert(values, key, data.value());
                }
            }
            cnx.upsert(relation.c_str(), keys, values);
            cnx.commit();
        } else {
            fostlib::json keys(calc_keys(m, m.configuration["PUT"]["keys"]));
            fostlib::json values(calc_values(body, m.configuration["PUT"]["attributes"]));
            cnx.upsert(relation.c_str(), keys, values).commit();
        }
        return get(cnx, config, m, req);
    }
    std::pair<boost::shared_ptr<fostlib::mime>, int> post(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json body{
            fostlib::json::parse(
                fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        fostlib::string relation = fostlib::coerce<fostlib::string>(m.configuration["POST"]["table"]);
        fostlib::json col_config = m.configuration["POST"]["columns"];
        fostlib::json values;
        for ( auto col_def = col_config.begin(); col_def != col_config.end(); ++col_def ) {
            const auto name = fostlib::coerce<fostlib::string>(col_def.key());
            const auto data = fostgres::datum(name, *col_def, m.arguments, body, req);
            if ( not data.isnull() ) {
                fostlib::insert(values, name, data.value());
            }
        }
        const fostlib::json &ret_cols = m.configuration["POST"]["returning"];
        std::vector<fostlib::string> returning;
        std::transform(ret_cols.begin(), ret_cols.end(), std::back_inserter(returning),
            [](const auto &s) { return fostlib::coerce<fostlib::string>(s); });
        if ( not returning.size() ) {
            returning.emplace_back("*");
        }
        auto result = fostgres::column_names(cnx.insert(relation.c_str(), values, returning));
        cnx.commit();
        return get(cnx, std::move(result), config, m, req);
    }
    std::pair<boost::shared_ptr<fostlib::mime>, int>  patch(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::json body{
            fostlib::json::parse(
                fostlib::coerce<fostlib::string>(
                    fostlib::coerce<fostlib::utf8_string>(req.data()->data())))};
        fostlib::string relation = fostlib::coerce<fostlib::string>(m.configuration["PATCH"]["table"]);
        if ( m.configuration["PATCH"].has_key("columns") ) {
            fostlib::json keys, values, col_config = m.configuration["PATCH"]["columns"];
            for ( auto col_def = col_config.begin(); col_def != col_config.end(); ++col_def ) {
                auto key = fostlib::coerce<fostlib::string>(col_def.key());
                auto data = fostgres::datum(key, *col_def, m.arguments, body, req);
                if ( (*col_def)["key"].get(false) ) {
                    // Key column
                    if ( not data.isnull() ) {
                        fostlib::insert(keys, key, data.value());
                    } else {
                        throw fostlib::exceptions::not_implemented(__func__,
                            "Key column doesn't have a value", key);
                    }
                } else if ( not data.isnull() ) {
                    // Value column
                    fostlib::insert(values, key, data.value());
                }
            }
            cnx.update(relation.c_str(), keys, values);
            cnx.commit();
        } else {
            fostlib::log::warning(fostgres::c_fostgres)
                ("", "PATCH configuration with 'keys' and 'attributes' is deprecated. Use 'columns'")
                ("configuration", m.configuration["PATCH"]);
            fostlib::json keys(calc_keys(m, m.configuration["PATCH"]["keys"]));
            fostlib::json values(calc_values(body, m.configuration["PATCH"]["attributes"]));
            cnx.update(relation.c_str(), keys, values).commit();
        }
        return get(cnx, config, m, req);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  del(
        fostlib::pg::connection &cnx,
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        auto get_result = get(cnx, config, m, req);
        auto sql = fostlib::coerce<fostlib::string>(m.configuration["DELETE"]);
        auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(sql));
        sp.exec(m.arguments);
        cnx.commit();
        return get_result;;
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  response_object(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::pg::connection cnx(fostgres::connection(config, req));
        if ( req.method() == "GET" or req.method() == "HEAD" ) {
            return get(cnx, config, m, req);
        } else if ( req.method() == "PATCH" ) {
            return patch(cnx, config, m, req);
        } else if ( req.method() == "POST" ) {
            return post(cnx, config, m, req);
        } else if ( req.method() == "PUT" ) {
            return put(cnx, config, m, req);
        } else if ( req.method() == "DELETE" ) {
            return del(cnx, config, m, req);
        } else {
            throw fostlib::exceptions::not_implemented(__FUNCTION__,
                "Invalid HTTP method -- should return 405");
        }
    }


    const fostgres::responder c_csj("object", response_object);


}

