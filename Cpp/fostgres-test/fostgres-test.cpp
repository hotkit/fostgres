/*
    Copyright 2016 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fg.hpp"
#include <fost/main>
#include <fost/postgres>


using namespace fostlib;
namespace filesystem = boost::filesystem;


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
    if ( args.size() < 2 ) {
        o << "\nRun with:\n\n    fostgres-test dbname ...\n\n"
            << std::endl;
        return 2;
    }
    /// State used by the testing process as it runs
    std::vector<settings> loaded_settings;
    std::unique_ptr<fostlib::log::global_sink_configuration> loggers;
    fg::program script;
    try {
        /// Create the database
        json cnxconfig;
        const string dbname = args[1].value();
        {
            o << "Going to be using database " << dbname << std::endl;
            const std::vector<string> dbparam(1, dbname);
            auto cnxdb = pg::connection(cnxconfig);
            auto dbcheck = cnxdb.procedure("SELECT COUNT(datname) FROM pg_database "
                "WHERE datistemplate = false AND datname=$1").exec(dbparam);
            if ( coerce<int64_t>((*dbcheck.begin())[0]) ) {
                o << "Database found. Dropping " << dbname << std::endl;
                pg::dropdb(cnxconfig, dbname);
            } else {
                o << "Database not found" << std::endl;
            }
            pg::createdb(cnxconfig, dbname);
            insert(cnxconfig, "dbname", dbname);
        }
        o << "Creating database " << dbname << std::endl;
        auto cnx = pg::connection(cnxconfig);

        /// Loop through the remaining tasks and run SQL packages or requests
        for ( std::size_t argn{2}; argn < args.size(); ++argn ) {
            const auto filename = coerce<filesystem::path>(args[argn].value());
            const auto extension = filename.extension();
            if ( extension == ".fg" ) {
                o << "Loading script " << filename << std::endl;
                script = fg::program(filename);
            } else if ( extension == ".json" ) {
                o << "Loading configuration " << filename << std::endl;
                loaded_settings.emplace_back(filename);
            } else if ( extension == ".sql" ) {
                o << "Executing SQL " << filename << std::endl;
                auto sql = coerce<utf8_string>(utf::load_file(filename));
                cnx.exec(sql);
                cnx.commit();
            } else {
                o << "Unknown script type " << extension << " for " << filename << std::endl;
                return 3;
            }
        }

        script();

        /// When done and everything was OK, return OK
        return 0;
    } catch ( fostlib::exceptions::exception &e ) {
        o << e << std::endl;
    } catch ( std::exception &e ) {
        o << "Caught std::exception\n\n" << e.what() << std::endl;
    } catch ( ... ) {
        o << "Caught an unknown exception" << std::endl;
    }
    return 1;
}

