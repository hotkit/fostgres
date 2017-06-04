/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "updater.hpp"

#include <fostgres/csj_mime.hpp>
#include <fostgres/fostgres.hpp>

#include <fost/csj.parser.hpp>
#include <fost/json>
#include <fost/log>
#include <fost/parse/json.hpp>
#include <fost/push_back>


namespace {


    const fostgres::responder c_csj("csj", fostgres::response_csj);


    std::pair<boost::shared_ptr<fostlib::mime>, int>  get(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        const auto &get_conf = m.configuration["GET"];
        fostlib::pg::connection cnx(fostgres::connection(config, req));
        if ( get_conf.isarray() ) {
            std::vector<fostgres::multi_csj_mime::csj_generator> parts;
            for ( const auto &part_conf : get_conf ) {
                auto data = fostgres::select_data(cnx, part_conf, m, req);
                parts.emplace_back(
                    std::make_unique<fostgres::csj_mime>(
                        req.headers()["Accept"].value(),
                        std::move(data.first),
                        std::move(data.second)));
            }
            auto ret = std::make_pair(
                boost::shared_ptr<fostlib::mime>(
                    new fostgres::multi_csj_mime(
                        req.headers()["Accept"].value(),
                        std::move(parts))),
                200);
            return ret;
        } else {
            auto data = fostgres::select_data(cnx, get_conf, m, req);
            return std::make_pair(
                boost::shared_ptr<fostlib::mime>(
                    new fostgres::csj_mime(
                        req.headers()["Accept"].value(),
                        std::move(data.first),
                        std::move(data.second))),
                200);
        }
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  patch(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        auto logger = fostlib::log::debug(fostgres::c_fostgres);
        logger("", "CSJ PATCH");
        fostlib::json work_done{fostlib::json::object_t()};
        std::size_t records{};

        // We're going to need these items later
        fostlib::pg::connection cnx{fostgres::connection(config, req)};
        fostgres::updater handler(m.configuration["PATCH"], cnx, m, req);

        // Interpret body as UTF8 and split into lines. Ensure it's not empty
        fostlib::csj::parser data(fostlib::utf::u8_view(req.data()->data()));
        logger("header", data.header());

        // Parse each line and send it to the database
        for ( auto line(data.begin()), e(data.end()); line != e; ++line ) {
            handler.upsert(line.as_json());
            ++records;
        }
        cnx.commit();
        fostlib::insert(work_done, "records", records);
        boost::shared_ptr<fostlib::mime> response(
                new fostlib::text_body(fostlib::json::unparse(work_done, true),
                    fostlib::mime::mime_headers(), L"application/json"));
        return std::make_pair(response, 200);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  put(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        auto logger = fostlib::log::debug(fostgres::c_fostgres);
        logger("", "CSJ PUT");

        fostlib::pg::connection cnx{fostgres::connection(config, req)};
        fostgres::updater handler(m.configuration["PUT"], cnx, m, req);
        fostlib::json work_done{fostlib::json::object_t()};

        // Create a SELECT statement to collect all the associated keys
        // in the database. We need to SELECT across the keys not in
        // the body data and store the keys that are in the body data
        /// The bool is set to true when the key has been seen. Those still
        /// false by the end of the PUT need to be deleted.
        std::vector<std::pair<std::vector<fostlib::json>, bool>> dbkeys;
        std::vector<fostlib::string> key_names;
        {
            auto rs = select_data(cnx, m.configuration["PUT"]["existing"], m, req);
            for ( const auto &row : rs.second ) {
                std::vector<fostlib::json> keys;
                for ( std::size_t index{}; index != row.size(); ++index ) {
                    keys.push_back(row[index]);
                }
                dbkeys.push_back(std::make_pair(std::move(keys), false));
            }
            std::sort(dbkeys.begin(), dbkeys.end());
            key_names = std::move(rs.first);
            fostlib::insert(work_done, "selected", dbkeys.size());
            logger("selected", dbkeys.size());
        }

        // Process the incoming data and put it into the database. Also
        // record the keys seen
        {
            // Interpret body as UTF8 and split into lines. Ensure it's not empty
            fostlib::csj::parser data(fostlib::utf::u8_view(req.data()->data()));
            logger("header", data.header());
            std::size_t records{0};
            std::vector<fostlib::json> key_match;
            key_match.reserve(key_names.size());

            // Parse each line and send it to the database
            for ( auto line(data.begin()), e(data.end()); line != e; ++line ) {
                key_match.clear();
                auto inserted = handler.upsert(line.as_json());
                ++records;
                // Look to see if we had this data in the database before
                // and if so mark it as seen in the PUT body
                for ( const auto &k : key_names ) {
                    key_match.push_back(inserted.first[k]);
                }
                auto found = std::lower_bound(
                    dbkeys.begin(), dbkeys.end(), std::make_pair(key_match, false));
                if ( found != dbkeys.end() && found->first == key_match ) {
                    found->second = true;
                }
            }
            fostlib::insert(work_done, "records", records);
            logger("records", records);
        }

        // Look through the initial keys to find any that weren't in the
        // incoming data so the rows can be deleted
        {
            auto sql = fostlib::coerce<fostlib::string>(m.configuration["PUT"]["delete"]);
            auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(sql));
            std::vector<fostlib::json> keys(m.arguments.size() + key_names.size());
            std::transform(m.arguments.begin(), m.arguments.end(), keys.begin(),
                [&](const auto &arg) {
                    return fostlib::json(arg);
                });
            std::size_t deleted{0};
            for ( const auto &record : dbkeys ) {
                if ( not record.second ) {
                    // The record wasn't "seen" during the upload so we're
                    // going to delete it.
                    std::copy(record.first.begin(), record.first.end(), keys.begin() + m.arguments.size());
                    sp.exec(keys);
                    ++deleted;
                }
            }
            fostlib::insert(work_done, "deleted", deleted);
            logger("deleted", deleted);
        }

        cnx.commit();

        boost::shared_ptr<fostlib::mime> response(
                new fostlib::text_body(fostlib::json::unparse(work_done, true),
                    fostlib::mime::mime_headers(), L"application/json"));
        return std::make_pair(response, 200);
    }


    std::pair<boost::shared_ptr<fostlib::mime>, int>  del(
        const fostlib::json &config, const fostgres::match &m,
        fostlib::http::server::request &req
    ) {
        fostlib::pg::connection cnx{fostgres::connection(config, req)};
        auto sql = fostlib::coerce<fostlib::string>(m.configuration["DELETE"]);
        auto sp = cnx.procedure(fostlib::coerce<fostlib::utf8_string>(sql));
        sp.exec(m.arguments);
        cnx.commit();
        boost::shared_ptr<fostlib::mime> response(new fostlib::text_body(L""));
        return std::make_pair(response, 200);
    }


}


std::pair<boost::shared_ptr<fostlib::mime>, int>  fostgres::response_csj(
    const fostlib::json &config, const fostgres::match &m,
    fostlib::http::server::request &req
) {
    if ( req.method() == "GET" ) {
        return get(config, m, req);
    } else if ( req.method() == "PATCH" ) {
        return patch(config, m, req);
    } else if ( req.method() == "PUT" ) {
        return put(config, m, req);
    } else if ( req.method() == "DELETE" ) {
        return del(config, m, req);
    } else {
        throw fostlib::exceptions::not_implemented(__FUNCTION__,
            "Must use GET, HEAD, DELETE, PUT or PATCH");
    }
}

