# Seeds

Utility to generate the seeds.txt list that is compiled into the client
(see [src/chainparamsseeds.h](/src/chainparamsseeds.h) and other utilities in [contrib/seeds](/contrib/seeds)).

    python3 generate-seeds.py . > ../../src/chainparamsseeds.h

## Dependencies

Ubuntu, Debian:

    sudo apt-get install python3-dnspython

and/or for other operating systems:

    pip install dnspython

See https://dnspython.readthedocs.io/en/latest/installation.html for more information.