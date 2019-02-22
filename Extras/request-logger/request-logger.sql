CREATE TABLE request_log (
    id text NOT NULL,
    PRIMARY KEY (id),
    started timestamp with time zone DEFAULT now(),
    request_headers JSON NOT NULL,
    messages JSON NOT NULL,
    exception text NULL,
    status int NOT NULL,
    response_headers JSON NOT NULL
);
