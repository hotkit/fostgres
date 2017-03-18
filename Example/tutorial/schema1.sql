CREATE TABLE todo (
    name text,
    PRIMARY KEY (name)
);

CREATE TABLE task (
    todo_name text,
    item text,
    PRIMARY KEY (todo_name, item)
);
