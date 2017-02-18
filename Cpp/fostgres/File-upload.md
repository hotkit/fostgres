# File uploads with Fostgres

In `PATCH`, `PUT` and `POST` requests files can be uploaded (see [`fostgres.sql` view reference](./Fostgres-sql-view.md) for details about how to configure the view).

Two media types are allowed for the `"object"` response type:

* `multipart/form-data` -- The form data is embedded in the request body as binary data. Care should be taken by the client to choose a suitable boundary string. *NB Not yet implemented*
* `application/json` and `application/csj` -- The file data is assumed to be base 64 encoded in a string in the relevant field source. See "Decoding a file from a JSON object field" below.

For normal APIs (multi-row) the field should always be base 64 encoded data.

## Column configuration

For file uploads the column configuration must contain a full configuration telling

* `type` -- Must have the value of `file` for file uploads.
* `store` -- The name of the file storage that is to be used (see File storage configuration below). * Future extension* If not specified then the file data is passed on to the database for storage.

An example column configuration is below:

    "avatar": {
        "type": "file",
        "store": "uploads"
    }

The column value that will be sent to the database is the store relative path name. If the store path is specified to be `/var/lib/site/uploads` then the filename might be `1234/123456789`. The actual filename used will be based on the content of the file, this ensures that the same uploads will always be placed at the same location.


## File store configuration

This is configured under the global setting `File storage`, with the setting name being the name of the file storage. For example, a file store called `uploads` might be configured in a JSON file as:

    {"File storage": {
        "uploads": {
            "path": "/var/lib/site/uploads"
        }
    }

This JSON file would then be passed as a parameter to the `fost-webserver` when it is run.

The storage configuration itself consists of:

* `path` -- The root of the file storage location. All of the files will be placed in this folder.


## Decoding a file from a JSON object field

If the JSON field is a stirng then it is assumed to be the base 64 encoded file content.


## Security considerations

Allowing users to upload files to your servers is a dangerous thing to do, both for your servers and for other users of your web site -- some care needs to be taken.

Complex media types should be checked for compromises. This includes image formasts, PDFs and documents.

## Garbage collection of old files

Fostgres will create new files in response to file data in requests, but it will never remove any files. Removal of files that are no longer referenced from the database is the responsiblity of the
