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


[Continue the tutorial](./page1.md)

