# fostgres
RESTful APIs for Postgres databases


# Views and configuration

Most of Fostgres is implemented through a single view, `fostgres.sql` which is suitlable for general interaction with the database.

## Configuration for running the tests target

You will need Postgres 9.5 or later installed. The database needs to be asccesible to your user account with super user permissions. Running `psql` on its own (with no command line options) will allow you to run commands like `CREATE DATABASE`.

