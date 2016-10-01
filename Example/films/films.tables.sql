CREATE TABLE films (
    slug text NOT NULL,
    title text NOT NULL,
    released date NOT NULL,
    length_minutes int NULL,
    created timestamp with time zone DEFAULT current_timestamp,
    CONSTRAINT filsm_pk PRIMARY KEY(slug)
);


CREATE TABLE film_tags (
    film_slug text NOT NULL REFERENCES films ON DELETE CASCADE INITIALLY DEFERRED,
    slug text NOT NULL,
    CONSTRAINT film_tags_pk PRIMARY KEY (film_slug, slug)
);


CREATE TABLE film_files (
    filepath text NOT NULL,
    film_slug text NOT NULL REFERENCES films ON DELETE CASCADE INITIALLY DEFERRED,
    CONSTRAINT film_files_pk PRIMARY KEY(filepath)
);


CREATE TABLE film_watched (
    film_slug text NOT NULL REFERENCES films ON DELETE CASCADE INITIALLY DEFERRED,
    watched timestamp with time zone DEFAULT current_timestamp,
    who text NOT NULL,
    CONSTRAINT film_watched_pk PRIMARY KEY (film_slug, watched)
);


CREATE VIEW films_view AS
    SELECT films.*,
            max(film_watched.watched) AS watched__last,
            count(film_watched.film_slug) AS watched__times,
            array_to_json(ARRAY
                    (SELECT slug FROM film_tags
                        WHERE film_tags.film_slug=films.slug
                        ORDER BY slug)
                ) AS tags
        FROM films
        LEFT JOIN film_watched ON film_watched.film_slug=films.slug
        GROUP BY films.slug;
