CREATE TABLE users (
    username text NOT NULL,
    email text NOT NULL,
    CONSTRAINT users_pk PRIMARY KEY(username)
);


CREATE TABLE passwords (
    username text NOT NULL REFERENCES users
        ON DELETE CASCADE INITIALLY DEFERRED,
    hashed text NOT NULL,
    salt text NOT NULL,
    CONSTRAINT passwords_pk PRIMARY KEY(username)
);


CREATE TABLE avatar_upload (
    username text NOT NULL REFERENCES users
        ON DELETE CASCADE INITIALLY DEFERRED,
    avatar text NOT NULL,
    CONSTRAINT avatar_upload_pk PRIMARY KEY (username)
);


CREATE TABLE groups (
    name text NOT NULL,
    CONSTRAINT groups_pk PRIMARY KEY (name)
);


CREATE TABLE permissions (
    name text NOT NULL,
    CONSTRAINT permissions_pk PRIMARY KEY (name)
);


CREATE TABLE group_permissions (
    group_name text NOT NULL,
    permission_name text NOT NULL,
    CONSTRAINT group_permissions_pk
        PRIMARY KEY (group_name, permission_name)
);

