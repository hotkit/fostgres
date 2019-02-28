CREATE TABLE request_log (
    id text NOT NULL,
    PRIMARY KEY (id),
    started timestamp with time zone DEFAULT now(),
    request_headers JSON NOT NULL,
    request_path text NOT NULL,
    messages JSON NOT NULL,
    duration float NOT NULL,
    exception text NULL,
    status int NULL,
    response_headers JSON NULL
);
