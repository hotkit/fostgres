# fostgres

*RESTful APIs for Postgres databases*

Postgres is an incredibly powerful database system. It seems a shame to hide it behind a cripplingly bad O/RM just because you want to use it for your web APIs.

Fostgres is a very small wrapper around the SQL interfaces already present in Postgres. Its intention is to enable as much of Postgres' power as possible to be delivered over an HTTP interface. To actually make you want to use it though it also needs to make those APIs great to consume.

Yes, [the Fostgres name is stupid](http://tvtropes.org/pmwiki/pmwiki.php/Main/LampshadeHanging). Once somebody suggests a better one it'll get changed.


# Central concepts

In Fostgres a URL represents some data in the database (-- who'd have thought?) This is going to be either:

1. A single database row where the primary key (or some other unique constraint) appears in the URL.
2. A set of database rows (either all for a given table, or some sub-set where a filter appears in the URL).

The behaviour between these two wants to be slightly different. In the first the `SELECT` used to get the data needs to return exactly one row. Too few and it's a 404 and too many and there's a mismatch between your database schema and the URL configuration (an application error).

The second form can return as many or as few rows as the `SELECT` likes. The only difference is how much data ends up in the response body.

To look at something concrete let's think about a database that holds films and allows them to be tagged. The first case wants a configuration something like this:

    {
        "return": "object",
        "path": ["/film", 1],
        "GET": "SELECT * FROM films WHERE slug = $1"
    }

This means:

* We are going to return a JSON ojbect made up from the colums in the `SELECT`.
* The path is going to have the word "film" followed by what is going to become the first parameter in the `SELECT` statement. It should look something like `/film/terminator`
* The GET will issue the specified SELECT and then return the object (or a 404 if nothing is matched).

The other way we want to handle things is to return multiple rows. For example, fetching all of the tags in the database:

    {
        "path": ["/tags"],
        "GET": "SELECT * FROM tags"
    }

This means:

* We don't specify a "return" because this form is the default.
* The path just contains `/tags` and there are no parameters to the SQL.
* The `SELECT` statement that will be run for the GET request.

This should be enough to understand at least some of the [configuration](./tests/config/view.film-slug.json) found in the [films](./tests/schema/films.tables.sql) example.


# Views and configuration

Most of Fostgres is implemented through a single view, `fostgres.sql` which is suitable for general interaction with the database.

## Configuration for running the `test`s target

You will need Postgres 9.5 or later installed. The database needs to be asccesible to your user account with super user permissions. Running `psql` on its own (with no command line options) will allow you to run commands like `CREATE DATABASE`.

