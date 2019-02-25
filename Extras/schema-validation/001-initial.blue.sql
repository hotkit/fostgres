CREATE TABLE json_schema (
    slug text NOT NULL,
    PRIMARY KEY (slug),
    schema json NOT NULL
);
