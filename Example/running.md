# Running Fostgres

The simplest way to run Fostgres is through [Docker](https://www.docker.com/), although it can also be built from source.

There is a docker image available, `kayess/fostgres` which can be run. You will need to make Postgres available to it.


## Testing the image

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

