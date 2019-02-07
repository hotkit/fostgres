# Fostgres Tutorial--Part 1

Our starting schema is this:

    CREATE TABLE todo (
        name text,
        PRIMARY KEY (name)
    );

    CREATE TABLE task (
        todo_name text,
        item text,
        PRIMARY KEY (todo_name, item)
    );

This is a Fostgres rather than an SQL modelling tutorial, but we can anyway see that this schema isn't great for what we're trying to do. They are however enough for us to be able to start with some APIs and tests to see why they're awful.

We'll also need a test script. To start with we'll just insert some data into the database.

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

Let's run this into a database called `todo`.

    fostgres-test todo schema1.sql tests1.fg

This will print something like the following:

    Fostgres testing environment
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

This last line means we want to test a view called `todo-list` and fetch a URL `/Create%20tutorial`. This GET request should give us a 200 response status. To actually get anything we'll also need to have a view. Let's build this up in stages:

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

    fostgres-test todo fostgres.so schema1.sql views1.json tests3.fg
    Fostgres testing environment
    Copyright (C) 2016, Felspar Co. Ltd.
    Going to be using database todo
    Database found. Dropping todo
    Creating database todo
    Loading library libfostgres.so
    Executing SQL schema1.sql
    Loading configuration views1.json
    Loading script tests3.fg

As we would expect, the GET for the row we entered works and the GET for a row we didn't returns a 404. Next we can create an API that allows us to create a to do list using PUT.

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

    GET todo-list /Create%20tutorial 200
    GET todo-list /Not-a-list 404

    PUT todo-list /New%20list {} 200

This last line simulates a PUT request. The body is empty (we only have one column and it's value is the URL) and we expect a 200 to come back. If we run this we'll get an error because we haven't defined the view yet.

    {"webserver": {
        "views/todo-list": {
            "view": "fostgres.sql",
            "configuration": {
                "sql": {
                    "return": "object",
                    "path": [1],
                    "GET": "SELECT * from todo WHERE name=$1",
                    "PUT": {
                        "table": "todo",
                        "columns": {
                            "name": {
                                "key":  true,
                                "source": 1
                            }
                        }
                    }
                }
            }
        }
    }}

The PUT configuration is quite different to the GET configuration. This is because Fostgres has to build the correct SQL statement to manage the data, and this depends not only on the configuration that it is given, but also on the data in the request body. Fostgres will generate a `INSERT ON CONFLICT UPDATE` statement, so the data must always be insertable, but it also means that the PUT request can be used for both row creation and row editing.

Right now Fostgres doesn't try to detect if the `INSERT` or the `UPDATE` part of the statement actually ran so it will never return a 201 (which could signify the resource was created rather than edited) to the client. In any case, there is no way that the client can perform different actions based on this difference in status code and remain idempotent -- a key aspect of PUT requests.

We can now see that after the tests run the new data is also in the database:

    $ psql todo
    psql (9.5.5)
    Type "help" for help.

    todo=# select * from todo; select * from task;
        name
    -----------------
    Create tutorial
    New list
    (2 rows)

        todo_name    |    item
    -----------------+------------
    Create tutorial | First step
    (1 row)

We can also check this from the script by adding a GET:

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

    GET todo-list /Create%20tutorial 200
    GET todo-list /Not-a-list 404

    PUT todo-list /New%20list {} 200
    GET todo-list /New%20list 200 {"name": "New list"}

This time the GET check also makes sure that the returned JSON object contains a `name` key which has the right value in it.

## Primary keys

The way that primary keys map to URLs is fundemental in understanding how the Fostgres configuration and your API design relate to the schema.


## Second API

Let's look at the to do items themselves. Firstly, let's add an API for returning all of the items on a given list.

    sql.insert todo {"name": "Create tutorial"}
    sql.insert task {"todo_name": "Create tutorial", "item": "First step"}

    GET todo-list /Create%20tutorial 200
    GET todo-list /Not-a-list 404

    PUT todo-list /New%20list {} 200
    GET todo-list /New%20list 200 {"name": "New list"}

    GET todo-list /Create%20tutorial/items 200

Running this gives an error because we only have one API and that will only match a URL that contains a single part. This last URL contains two parts. We need to add a second end point to our configuration. We do this by using an array of objects that contain the different `path` options we want to match against:

    {"webserver": {
        "views/todo-list": {
            "view": "fostgres.sql",
            "configuration": {
                "sql": [
                    {
                        "return": "object",
                        "path": [1],
                        "GET": "SELECT * FROM todo WHERE name=$1",
                        "PUT": {
                            "table": "todo",
                            "columns": {
                                "name": {
                                    "key":  true,
                                    "source": 1
                                }
                            }
                        }
                    },
                    {
                        "path": [1, "/items"],
                        "GET": "SELECT * FROM task WHERE todo_name=$1"
                    }
                ]
            }
        }
    }}

This second API now has a string in the `path` component. This tells Fostgres that the first part of the URL is going to be bound to the first input in the SQL and the second part must match the string `/items`. The SELECT can now use the first parameter to return all of the to do items associated with the list. Our test now passes, but we haven't checked that it returns the right data.

Because this SQL can return many rows we don't have the `"return": "object"` in the configuration. If we added it we would get an error the first time the API returned more than one row. Instead we get Fostgres' default return type which is CSJ. [CSJ is almost the same as CSV](http://www.kirit.com/Comma%20Separated%20JSON), but because it is based on JSON syntax it doesn't have the ambiguity that we would have with CSV.
