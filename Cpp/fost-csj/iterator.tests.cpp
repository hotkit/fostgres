/*
    Copyright 2016, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/csj.iterator.hpp>
#include <fost/test>


FSL_TEST_SUITE(csj_iterator);


FSL_TEST_FUNCTION(one_line) {
    fostlib::csj::line_iterator iter("1,2,3,4,5\n");
    auto line = *iter;
}

