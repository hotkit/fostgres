/**
    Copyright 2015-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include <fost/core>
#include <fost/test-throw-view.hpp>

#include <pqxx/except>


namespace {


    fostlib::urlhandler::test_throw_plugin const c_serialisation{
            "pqxx::serialization_failure", [](fostlib::string msg) {
                throw pqxx::serialization_failure{static_cast<std::string>(msg),
                                                  "SQL query", "40001"};
            }};


}
