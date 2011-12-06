Avalanche Server
================
server.cpp instantiates an avalanche::server (defined in lib), and uses it to publish a TH1F object.

Note: Be sure the avalanche shared library (in lib/cpp) is in your LD_LIBRARY_PATH.

Building
--------
To build, run:

    $ AVALANCHE=PATH/TO/AVALANCHE/LIB make

