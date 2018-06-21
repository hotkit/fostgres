# Running Fostgres

The simplest way to run Fostgres is through [Docker](https://www.docker.com/), although it can also be built from source.

There is a docker image available, `kayess/fostgres` which can be run. You will need to make Postgres available to it.


## The `docker run` command

Assuming you're running Postgres on your host machine and it has a unix domain socket available in `/var/run/postgres` (the default on Ubuntu) you can run the image using:

    sudo docker run -it \
        -v/var/run/postgresql:/var/run/postgresql \
        -v$(pwd):/src \
        -w/src \
        -u$(id -u):$(id -g) \
        -ePGUSER=$USER \
        kayess/fostgres \
        fostgres-test todo schema1.sql tests1.fg

This breaks down as:

* `-it` Run the container interactively
* `-v/var/run/postgresql:/var/run/postgresql` Share the host Postgresql unix domain socket to the container.
* `-v$(pwd):/src` Make the current directory available inside the container in the `/src` folder.
* `-w/src` Use the /src folder as the current directory in the container.
* `-u$(id -u):$(id -g)` Use the currently logged in user and group IDs inside the container.
* `-ePGUSER=$USER` Use the current username as the Postgresql role name when connecting.
* `kayess/fostgres` The container name
* `fostgres-test todo schema1.sql tests1.fg` The command to run in the container.


# Testing the image

Make sure we have the latest version of the image:

    sudo docker pull kayess/fostgres

Clone the Fostgres project files:

    git clone git@github.com:KayEss/fostgres.git

There are a number of examples that can be used, this is one of the tests:

    cd fostgres/Examples/films

And finally run the image:

    sudo docker run -it \
        -v/var/run/postgresql:/var/run/postgresql \
        -v$(pwd):/src \
        -u$(id -u):$(id -g) \
        -w/src \
        -ePGUSER=$USER \
        kayess/fostgres:latest \
        fostgres-test fostgres-test-films \
            libfostgres.so films.fg films.tables.sql view.film-slug.json

It's probably most convenient to set up an alias for this:

    alias fostgres-test="sudo docker run -it
        -v/var/run/postgresql:/var/run/postgresql
        -v$(pwd):/src
        -u$(id -u):$(id -g)
        -w/src
        -ePGUSER=$USER
        kayess/fostgres:latest
        fostgres-test"

## Troubleshooting

**TODO**

