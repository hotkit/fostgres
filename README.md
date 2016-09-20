# fostgres

*RESTful APIs for Postgres databases*

Postgres is an incredibly powerful database system. It seems a shame to hide it behind a cripplingly bad O/RM just because you want to use it for your web APIs.

Fostgres is a very small wrapper around the SQL interfaces already present in Postgres. Its intention is to enable as much of Postgres' power as possible to be delivered over an HTTP interface. To actually make you want to use it though it also needs to make those APIs great to consume.

Yes, the Fostgres name is stupid. Once somebody suggests a better one it'll get changed.


# Views and configuration

Most of Fostgres is implemented through a single view, `fostgres.sql` which is suitable for general interaction with the database.

## Configuration for running the `test`s target

You will need Postgres 9.5 or later installed. The database needs to be asccesible to your user account with super user permissions. Running `psql` on its own (with no command line options) will allow you to run commands like `CREATE DATABASE`.

