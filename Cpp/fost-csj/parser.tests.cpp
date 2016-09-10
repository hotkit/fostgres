/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.parser.hpp>
#include <fost/test>


FSL_TEST_SUITE(csj_parser);


FSL_TEST_FUNCTION(one_line) {
    fostlib::utf8_string str("\"h1\",\"h2\",\"h3\",\"h4\",\"h5\"\n1,2,3,4,5");
    fostlib::csj::parser csj(str);
    auto line = *(csj.begin());
    FSL_CHECK_EQ(line.size(), 5u);
    auto iter = line.begin();
    FSL_CHECK_EQ(*(iter++), fostlib::json("h1"));
}

