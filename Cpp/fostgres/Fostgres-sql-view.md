# `fostgres.sql` view reference

* For details about file uploads see [File uploads](./File-uploads.md)


## Configuration

* `sql` -- The individual end points, see "End point configuration" below.

The view first goes through the list of end point configurations looking for the first path match.


### End point configuration

Each end point configuration consists of a number of keys:

* `response` -- The type of API. The URL to database mapping essentially must describe a resource that comprised either one or multiple rows in the relation/table.
    * `object` -- The URL describes a single row in the database.
    * `csj` (default) -- The URL describes multiple rows in the database.
* `path` -- The path configuration to match against
* `GET` -- Used for `GET` requests.
* `PUT` -- Used for `PUT` requests.
* `PATCH` -- Used for `PATCH` requests.
* `POST` -- Used for `POST` requests.
* `DELETE` -- Used for `DELETE` requests.

#### Path configuration

The path is a JSON array of path elements that are to be matched against. Two options exist:

* A number. This then becomes that source number in the SQL. For example, `1` would become the `$1` argument in the `GET` request SQL.
* A string starting with a forward slash. This describes fixed text in the URL.

For example:

    ["/location", 1]

Would match the following URLs:

    /location/one
    /location/foo
    /location/bar.txt

But would not match:

    /location/one/two
    /api/location/one

To match the first one use one of:

    ["/location", 1, 2] (for two arguments)
    ["/location", 1, "/two"] (for one argument)

And again, the second could have a one or two argument form:

    ["/api", "/location", 1] (for one argument)
    [2, "/location", 1] (for two arguments)

