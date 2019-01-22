CREATE TABLE users (
    username text NOT NULL,
    email text NOT NULL UNIQUE,
    CONSTRAINT users_pk PRIMARY KEY(username)
);

CREATE TABLE friends (
    username text NOT NULL REFERENCES users ON DELETE CASCADE INITIALLY DEFERRED,
    friend text NOT NULL REFERENCES users ON DELETE CASCADE INITIALLY DEFERRED,
    PRIMARY KEY (username, friend),
    friend_type TEXT NOT NULL
);
