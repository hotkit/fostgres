# Testing function reference


## contains

Ensures that the `data` has all of the parts that are provided in the `checks` JSON.

    contains data check

For each key path in the `check` the value must be present in `data`.

    set d {"k1": "Hello", "k2": {"foo": "bar", "baz": [1, 2, 3]}}
    contains (lookup d) {"k1": "Hello"} # Ok, k1 has the value "Hello"
    contains (lookup d) {"k3": null} # Fails as there is no k3 key
    contains (lookup d) {"k2": {"foo": "bar"}} # OK, k2's object has a foo key with the value "bar"
    contains (lookup d) {"k2": {"baz": [1, 2, 3]}} # OK because the array at k2.baz matches
    contains (lookup d) {"k2": {"baz": [2, 3, 1]}} # Fails because the array elements are out of order


## GET

Check a GET request.

    GET view path expected-status [expected-body]

The expected body is checked using the `contains` algorithm.

## PUT

Perform a PUT request.

    PUT view path body expected-status [expected-body]

The expected body is checked using the `contains` algorithm.
