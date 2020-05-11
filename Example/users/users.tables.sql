CREATE TABLE users (
    username text NOT NULL,
    email text NOT NULL,
    CONSTRAINT users_pk PRIMARY KEY(username)
);


CREATE TABLE passwords (
    username text NOT NULL REFERENCES users ON DELETE CASCADE INITIALLY DEFERRED,
    hashed text NOT NULL,
    salt text NOT NULL,
    CONSTRAINT passwords_pk PRIMARY KEY(username)
);


CREATE TABLE avatar_upload (
    username text NOT NULL REFERENCES users ON DELETE CASCADE INITIALLY DEFERRED,
    avatar text NOT NULL,
    CONSTRAINT avatar_upload_pk PRIMARY KEY (username)
);


CREATE TABLE administrators (
    username text NOT NULL REFERENCES users ON DELETE CASCADE INITIALLY DEFERRED
);
