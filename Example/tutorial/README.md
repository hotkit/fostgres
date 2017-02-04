# Fostgres tutorial

In this tutorial we're going to create a database schema, a set of APIs and test them. It assumes that you have access to a Fostgres build and that you have a Postgres server you can use.

We're going to build a simple to-do system. Not becuase it's a particularly interested problem, but rather because it isn't. It'll allow us to focus on the key aspects important to Fostgres:

1. What does a good database schema look like.
2. What do a set of good RESTful APIs look like.
3. How can we test that we will get the right results.

There are two main components, the to-do list and the to-do items on the list. We can start with this very simple schema and then make improvements until we have something we like:

    CREATE TABLE todo (
        name text,
        PRIMARY KEY (name)
    );

    CREATE TABLE task (
        todo_name text,
        item text,
        PRIMARY KEY (todo_name, item)
    );

This is a Fostgres rather than an SQL modelling tutorial, but we can anyway see why these tables are awful. They are however enough for us to be able to start with some APIs and tests to see why they're awful.

We'll also need a test script. To start with we'll just insert some data into the database.

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

Let's run this into a database called `todo`.

    fostgres-test todo schema1.sql tests1.fg

This will print something like the following:

    Fostgres testing environment
    Copyright (C) 2016, Felspar Co. Ltd.
    Going to be using database todo
    Database not found
    Creating database todo
    Executing SQL schema1.sql
    Loading script tests1.fg

And we can look into the database after the tests have run and see our data in there.

    $ psql todo
    psql (9.5.5)
    Type "help" for help.

    todo=# select * from todo; select * from task;
        name
    -----------------
    Create tutorial
    (1 row)

        todo_name    |    item
    -----------------+------------
    Create tutorial | First step
    (1 row)

## First API

Let's see if we can retrieve the todo list. Change the test script:

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

    GET todo-list /Create%20tutorial 200

This last line means we want to test a view called `todo` and fetch a URL `/Create%20tutorial`. This GET request should give us a 200 response status. To do actually get anything we'll also need to have a view. Let's build this up in stages:

    {"webserver": {
    }}

This part says we want to configure part of the web server. Next we say which view name we want:

    {"webserver": {
        "views/todo-list": {
        }
    }}

The view is called `todo-list` and you'll remember this is what we used in the test script. The web server can run many different views, but for Fostgres we want to run the `fostgres.sql` view.:

    {"webserver": {
        "views/todo-list": {
            "view": "fostgres.sql",
            "configuration": {
            }
        }
    }}

We're also going to need to give it a configuration.

    {"webserver": {
        "views/todo-list": {
            "view": "fostgres.sql",
            "configuration": {
                "sql": {
                    "return": "object",
                    "path": [1],
                    "GET": "SELECT * from todo WHERE name=$1"
                }
            }
        }
    }}

We're only going to focus on the SQL part of the configuration. This API is going to return a JSON object, this is the `return": "object"`. We also give it an SQL statement to run for the GET requests, this is the `"GET": "SELECT * from todo WHERE name=$1"` part. The `"path": [1]` means that the first part of the URL path (between forward slashes) will become the `$1` in the SQL.

In practice the `return": "object"` really means that the API is talking about a resource that is a single row in the database. We can also talk about resources that describe more than one row. Before going there though let's run our test and make sure it passes. We'll change our test script a little bit first:

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

    GET todo-list /Create%20tutorial 200
    GET todo-list /Not-a-list 404


And then run it:

    fostgres-test todo libfostgres.so schema1.sql views1.json tests3.fg
    Fostgres testing environment
    Copyright (C) 2016, Felspar Co. Ltd.
    Going to be using database todo
    Database found. Dropping todo
    Creating database todo
    Loading library libfostgres.so
    Executing SQL schema1.sql
    Loading configuration views1.json
    Loading script tests3.fg

As we would expect, the GET for the row we entered works and the GET for a row we didn't returns a 404.


