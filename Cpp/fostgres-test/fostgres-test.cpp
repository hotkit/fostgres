/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/main>


using namespace fostlib;


namespace {
    const setting<string> c_host("fostgres-test.cpp",
        "fostgres-test", "Bind to", "localhost");
    const setting<int> c_port("fostgres-test.cpp",
        "fostgres-test", "Port", 8001);
    const setting<string> c_mime("fostgres-test.cpp",
        "fostgres-test", "MIME types", "Configuration/mime-types.json");
    const setting<json> c_load("fostgres-test.cpp",
        "fostgres-test", "Load", json::array_t());

    const setting<json> c_logger("fostgres-test.cpp",
        "fostgres-test", "logging", fostlib::json(), true);
    // Take out the Fost logger configuration so we don't end up with both
    const setting<json> c_fost_logger("fostgres-test.cpp",
        "fostgres-test", "Logging sinks", fostlib::json::parse("{\"sinks\":[]}"));
}


FSL_MAIN(
    L"fostgres-test",
    L"Fostgres testing environment\nCopyright (C) 2016, Felspar Co. Ltd."
)( fostlib::ostream &o, fostlib::arguments &args ) {
    return 0;
}

