/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/main>
#include <hello/hello-lib>


FSL_MAIN(
    "hello-world",
    "Hello world program example"
)( fostlib::ostream &out, fostlib::arguments &args ) {
    out << hello_world::text() << std::endl;
    return 0;
}
