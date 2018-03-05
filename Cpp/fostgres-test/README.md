# Testing Fostgres APIs


Fostgres includes a very simple scripting language for writing API tests. It's designed to be simple to write and read and doesn't even have any control flow.

For example:

    GET film.slug /t1 200 {
            "watched": {"times": 0, "last": null},
            "tags": [
                "robots"
            ]}
    PUT film.slug /t1/tag/action%20adventure {} 200 {"tag": "action adventure"}



## Concepts

The tests consist of some SQL scripts that establishes a schema (and optionally some starting data)

## Basics

There is also a [function reference](./Functions.md) and an extensions reference is coming soon.

