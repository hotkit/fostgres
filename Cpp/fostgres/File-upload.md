# File uploads with Fostgres

In `PATCH`, `PUT` and `POST` requests files can be uploaded (see [`fostgres.sql` view reference](./Fostgres-sql-view.md) for details about how to configure the view).

Two media types are allowed for the `"object"` response type:

* `multipart/form-data` -- The form data is embedded in the request body as binary data. Care should be taken by the client to choose a suitable boundary string. This media type can only be used
* `application/json` -- The file data is assumed to be base 64 encoded in a string in the relevant field source.

For normal APIs (multi-row) then field should always be base 64 encoded data.

### Column configuration

For file uploads the column configuration must contain a full configuration telling

* `type` -- Must have the value of `file` for file uploads.
* `store` -- The name of the file storage that is to be used (see File storage configuration below). * Future extension* If not specified then the file data is passed on to the database for storage.


### File store configuration

This is configured under the global setting `File storage`, with the setting name being the name of the file storage. For example, a file store called `uploads` might be configured in a JSON file as:

    {"File storage": {
        "uploads": {
        }
    }

This JSON file would then be passed as a parameter to the `fost-webserver` when it is run.
