/**
    Copyright 2019, Proteus Technologies Co Ltd. <https://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/
/**
    TODO: This capability is forked from Odin, but really needs to be
    pushed down to the fost-crypto library.
 */


#include "nonce.hpp"
#include <fost/crypto>
#include <chrono>


namespace {
    fostlib::string nonce() {
        const auto base64url = [](auto &&v) {
            fostlib::utf8_string b64u;
            for (const auto c : v) {
                if (c == '+')
                    b64u += '-';
                else if (c == '/')
                    b64u += '_';
                else if (c == '=')
                    return b64u;
                else
                    b64u += c;
            }
            return b64u;
        };
        const auto bytes = fostlib::crypto_bytes<24>();
        const auto b64 = fostlib::coerce<fostlib::base64_string>(
                std::vector<unsigned char>(bytes.begin(), bytes.end()));
        return base64url(b64).underlying().c_str();
    }
}


fostlib::string rqlog::reference() {
    const auto time = std::chrono::system_clock::now();
    const auto t_epoch =
            std::chrono::system_clock::to_time_t(time); // We assume POSIX
    return fostlib::string(std::to_string(t_epoch)) + "-" + nonce();
}
