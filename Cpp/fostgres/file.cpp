/**
    Copyright 2017-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/response.hpp>

#include <fost/crypto>
#include <fost/file>


fostlib::nullable<fostlib::json> fostgres::file_upload(
        f5::u8view name, fostlib::json const &defn, fostlib::json const &row) {
    if (defn["source"].isnull() && row.has_key(name)) {
        fostlib::base64_string value{
                fostlib::coerce<fostlib::string>(row[name])};
        auto const data = fostlib::coerce<std::vector<unsigned char>>(value);
        fostlib::digester hasher{fostlib::sha256};
        hasher << data;
        auto const pathname =
                fostlib::coerce<fostlib::hex_string>(hasher.digest());
        fostlib::fs::path const directory =
                static_cast<std::string>(pathname.substr(0, 3));
        fostlib::fs::path const filename =
                static_cast<std::string>(pathname.substr(3));
        auto store_config = fostlib::setting<fostlib::json>::value(
                "File storage",
                fostlib::coerce<fostlib::string>(defn["store"]));
        auto location =
                fostlib::coerce<fostlib::fs::path>(store_config["path"]);
        location /= directory;
        fostlib::fs::create_directories(location);
        location /= filename;
        fostlib::fs::ofstream save(location);
        save.write(reinterpret_cast<const char *>(data.data()), data.size());
        return fostlib::coerce<fostlib::json>(directory / filename);
    } else {
        throw fostlib::exceptions::not_implemented(
                __PRETTY_FUNCTION__, "File upload where `source` is specified");
    }
    return fostlib::null;
}
