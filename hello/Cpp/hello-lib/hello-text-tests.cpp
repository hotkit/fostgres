/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <hello/hello-lib>
#include <fost/test>


FSL_TEST_SUITE(hello_lib);


FSL_TEST_FUNCTION(text) {
    FSL_CHECK_EQ(hello_world::text(), L"Hello world!");
}
