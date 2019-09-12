/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fostgres/fg/fg.hpp>


namespace fg {


    /// Register that new builtins should be added to the runtime
    class register_builtins {
        std::function<void(frame &)> lambda;

      public:
        /// Register a lambda that will be created when the initial
        /// frame is built before executing a script.
        register_builtins(std::function<void(frame &)>);

        void operator()(frame &) const;
    };


}
