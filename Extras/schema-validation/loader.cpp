/**
    Copyright 2018, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/

#include <f5/json/schema.loaders.hpp>


namespace {


    const f5::json::schema_loader c_loader{"fostgres-db-schema",
        [](f5::u8view url, f5::json::value config) -> std::unique_ptr<f5::json::schema>
        {
            const auto prefix = fostlib::coerce<f5::u8view>(config["prefix"]);
            if ( url.starts_with(prefix) ) {
                throw fostlib::exceptions::not_implemented(__PRETTY_FUNCTION__);
            }
            return {};
        }};


}

