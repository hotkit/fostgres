# Request logging


## Configuration

The view is not included by default in the Fostgres views, so the `webserver` `Load` configuration needs to have the following added to it:

    fostgres-request-logger


### fostgres.request-logging


This middleware will capture information about the request processing into the `request_log` table in Postgres. The middleware will try to attach a `Fostgres-Request-ID` header to the response which will allow the information to be retrieved later on.

Information captured includes:

* Request start time and duration.
* Request headers.
* Exception information **or** response status code and headers.
* Log messages produced during request processing.


```json
{
    "view": "fostgres.request-logging",
    "configuration": {
        "view": *inner view configuration*
    }
}
```

If the middleware sees an exception it cannot attach a request ID header. This means you probably normally want to use this in conjunction with the exception handling view:

```json
{
    "view": "fostgres.request-logging",
    "configuration": {
        "view": "control.exception.catch",
        "configuration": {
            "try": *inner view configuration*,
            "catch": {
                "std::exception": "fost.response.500"
            }
        }
    }
}
```

