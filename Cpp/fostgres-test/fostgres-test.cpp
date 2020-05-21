/**
    Copyright 2016-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fostgres/db.hpp>
#include <fostgres/fg/fg.hpp>
#include <fostgres/fg/fg.testserver.hpp>
#include <fost/dynlib>
#include <fost/main>
#include <fost/postgres>

#include <pqxx/except.hxx>


namespace {
    const fostlib::setting<fostlib::string> c_host(
            "fostgres-test.cpp", "fostgres-test", "Bind to", "localhost");
    const fostlib::setting<int>
            c_port("fostgres-test.cpp", "fostgres-test", "Port", 8001);
    const fostlib::setting<fostlib::string>
            c_mime("fostgres-test.cpp",
                   "fostgres-test",
                   "MIME types",
                   "Configuration/mime-types.json");
    const fostlib::setting<fostlib::json>
            c_load("fostgres-test.cpp",
                   "fostgres-test",
                   "Load",
                   fostlib::json::array_t());

    /// If we need to output a file when the script runs successfully then
    /// the filename can be specified here.
    const fostlib::setting<fostlib::nullable<fostlib::string>> c_output(
            "fostgres-test.cpp", "fostgres-test", "Output", fostlib::null, true);

    const fostlib::setting<fostlib::nullable<fostlib::string>> c_db_host(
            __FILE__, "fostgres-test", "DB Host", fostlib::null, true);
    const fostlib::setting<fostlib::nullable<fostlib::string>> c_db_user(
            __FILE__, "fostgres-test", "DB User", fostlib::null, true);

    const fostlib::setting<fostlib::json> c_logger(
            "fostgres-test.cpp", "webserver", "logging", fostlib::json(), true);
    // Take out the Fost logger configuration so we don't end up with both
    const fostlib::setting<fostlib::json> c_fost_logger(
            "fostgres-test.cpp",
            "fostgres-test",
            "Logging sinks",
            fostlib::json::parse("{\"sinks\":[]}"));

    const fostlib::setting<fostlib::nullable<fostlib::string>> c_zoneinfo(
            "fostgres-test.cpp",
            "fostgres-test",
            "Zone info",
            fostlib::null,
            true);

    /// Print the exception information and return exception data
    /// after removing the backtrace
    fostlib::json
            print(fostlib::ostream &o,
                  fg::program const &script,
                  fostlib::exceptions::exception &e) {
        fostlib::json error = e.data();
        if (error.has_key(fostlib::jcursor{"fg", "backtrace"})) {
            auto backtrace = error["fg"]["backtrace"];
            auto printbt = [backtrace, &o, &e]() {
                /**
                 * Until we can properly identify the source code location
                 * this is the only display we should see.
                 */
                o << fostlib::transitional_stringify(e.message()) << '\n';
                /// The top of the stack trace will always be the `progn`
                /// that is wrapped around the entire script. We remove
                /// that as the user doesn't need to see it.
                fostlib::json::array_t filtered;
                for (std::size_t index{}; index < backtrace.size() - 1u;
                     ++index) {
                    filtered.push_back(backtrace[index]);
                }
                o << "Backtrace: " << filtered << std::endl;
            };
            if (backtrace.has_key(fostlib::jcursor{0, 0})) {
                auto const place = script.source_for(
                        fostlib::coerce<f5::u8view>(backtrace[0][0]));
                if (place) {
                    o << place->filename << ":"
                      << fostlib::transitional_stringify(e.message()) << '\n'
                      << place->source << '\n';
                } else {
                    printbt();
                }
            } else {
                printbt();
            }
            fostlib::jcursor{"fg", "backtrace"}.del_key(error);
            if (error["fg"] == fostlib::json::object_t{}) {
                fostlib::jcursor{"fg"}.del_key(error);
            }
        } else {
            o << fostlib::transitional_stringify(e.message()) << std::endl;
        }
        return error;
    }


}


FSL_MAIN("fostgres-test", "Fostgres testing environment")
(fostlib::ostream &o, fostlib::arguments &args) {
    if (args.size() < 2) {
        o << "\nRun with:\n\n    fostgres-test dbname ...\n\n"
          << "      -h     Postgres hostname or path\n"
          << "      -U     Postgres username\n"
          << "      -o     Output filename to write on success\n"
          << '\n';
        return 2;
    }
    args.commandSwitch("h", c_db_host);
    args.commandSwitch("U", c_db_user);
    args.commandSwitch("o", c_output);

    const auto success = [&o](const char *msg) {
        o << msg << std::endl;
        if (c_output.value()) {
            fostlib::utf::save_file(
                    fostlib::coerce<fostlib::fs::path>(c_output.value().value()),
                    "");
        }
        return 0;
    };


    /// State used by the testing process as it runs
    std::vector<fostlib::settings> loaded_settings;
    std::vector<std::unique_ptr<fostlib::dynlib>> dynlibs;
    fg::program script;
    try {
        /// Create the database
        fostlib::json cnxconfig;
        if (c_db_host.value())
            fostlib::insert(cnxconfig, "host", c_db_host.value().value());
        if (c_db_user.value())
            fostlib::insert(cnxconfig, "user", c_db_user.value().value());
        const fostlib::string dbname = args[1].value();
        {
            o << "Going to be using database " << dbname << std::endl;
            const std::vector<fostlib::string> dbparam(1, dbname);
            auto cnxdb = fostgres::connection(cnxconfig, c_zoneinfo.value());
            auto dbcheck =
                    cnxdb.procedure(
                                 "SELECT COUNT(datname) FROM pg_database "
                                 "WHERE datistemplate = false AND datname=$1")
                            .exec(dbparam);
            if (fostlib::coerce<int64_t>((*dbcheck.begin())[0])) {
                o << "Database found. Dropping " << dbname << std::endl;
                fostlib::pg::dropdb(cnxconfig, dbname);
            } else {
                o << "Database not found" << std::endl;
            }
            fostlib::pg::createdb(cnxconfig, dbname);
            insert(cnxconfig, "dbname", dbname);
        }
        o << "Creating database " << dbname << std::endl;
        auto cnx = fostgres::connection(cnxconfig, c_zoneinfo.value());

        /// Loop through the remaining tasks and run SQL packages or requests
        for (std::size_t argn{2}; argn < args.size(); ++argn) {
            const auto filename =
                    fostlib::coerce<fostlib::fs::path>(args[argn].value());
            const auto extension = filename.extension();
            if (extension == ".fg") {
                o << "Loading script " << filename << std::endl;
                script = fg::program(filename);
            } else if (extension == ".json") {
                o << "Loading configuration " << filename << std::endl;
                loaded_settings.emplace_back(filename);
            } else if (extension == ".so" || extension == ".dylib") {
                o << "Loading library " << filename << std::endl;
                dynlibs.emplace_back(
                        std::make_unique<fostlib::dynlib>(args[argn].value()));
            } else if (extension == ".sql") {
                o << "Executing SQL " << filename << std::endl;
                auto sql = fostlib::coerce<fostlib::utf8_string>(
                        fostlib::utf::load_file(filename));
                cnx.exec(sql);
                cnx.commit();
            } else {
                o << "Unknown script type " << extension << " for " << filename
                  << std::endl;
                return 3;
            }
        }

        // Set up the logging options
        std::unique_ptr<fostlib::log::global_sink_configuration> loggers;
        if (not c_logger.value().isnull()
            && c_logger.value().has_key("sinks")) {
            loggers = std::make_unique<fostlib::log::global_sink_configuration>(
                    c_logger.value());
        }

        // Run the script
        fg::frame stack(fg::builtins());
        stack.symbols["pg.dsn"] = cnxconfig;
        stack.symbols["pg.zoneinfo"] = c_zoneinfo.value();
        script(stack);

        /// When done and everything was OK, return OK
        return success("Test script passed");
    } catch (fg::program::empty_script &) {
        return success("No commands were found in the script");
    } catch (fg::program::nothing_loaded &) {
        o << "No script was specified on the command line" << std::endl;
        return 4;
    } catch (fg::mismatched_status_code &e) {
        print(o, script, e);
        o << "Expected " << e.expected << " but got a response of " << e.actual
          << '\n'
          << std::endl;
    } catch (fostlib::exceptions::exception &e) {
        auto const error = print(o, script, e);
        if (error.size()) { o << error << std::endl; }
    } catch (pqxx::sql_error &e) {
        o << "Postgres error " << e.sqlstate() << std::endl;
        o << e.what() << std::endl;
    } catch (std::exception &e) {
        o << "Caught std::exception " << typeid(e).name() << "\n\n"
          << e.what() << std::endl;
    } catch (...) { o << "Caught an unknown exception" << std::endl; }
    return 1;
}
