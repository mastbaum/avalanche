Avalanche Client Library Examples
=================================
Examples demonstrating usage of the avalanche library.

Building
--------
To build, run:

    $ AVALANCHE=PATH/TO/AVALANCHE make

client.cpp
----------
The client listens for incoming data on both a dispatcher stream and from a CouchDB database. It prints the data type, and if an event (RAT::DS::Root) is received, prints the NHITs for the event.

watch_db.cpp
------------
Watch a CouchDB changes feed for documents convertible to RAT::DS objects, and print the contents of the converted TObjects. Useful for debugging JSON-to-TObject mapping functions.

Usage:

    ./watch_db http://example.com:5984 db_name

