# Building Fostgres

Fostgres is hosted in the Mengmom web server and is a standard plug-in to it, so is built as part of that project.

    git clone --recursive git@github.com:KayEss/mengmom.git
    cd mengmom

You will need to build the libraries that are used. This is only needed one time.

    Boost/build
    PostgreSQL/configure

Finally we can do a release build using gcc:

    mengmom/compile toolset=gcc release fostgres odin

This will produce a `dist` folder inside of which is everything that has been built.


## Testing the build

The below assumes you're running from the same folder where the build was done.

    LD_LIBRARY_PATH=dist/lib \
        ./dist/bin/fostgres-test \
        fostgres-test-films \
        libfostgres.so \
        fostgres/Example/films/films.fg \
        fostgres/Example/films/films.tables.sql \
        fostgres/Example/films/view.film-slug.json

You should get output similar to:

    Fostgres testing environment
    Going to be using database fostgres-test-films
    Database not found
    Creating database fostgres-test-films
    Loading library libfostgres.so
    Loading script fostgres/Example/films/films.fg
    Executing SQL fostgres/Example/films/films.tables.sql
    Loading configuration fostgres/Example/films/view.film-slug.json
    Test script passed


## Troubleshooting the build

**TODO**

