Publish-Subscribe Model for RAT Packed Record Delivery
======================================================
This client code receives PackedRec objects published by the RAT DispatchEvents processor.

Note: Be sure the avalanche shared library (in lib/cpp) is in your LD_LIBRARY_PATH.

Building
--------
To build, run:

    $ AVALANCHE=PATH/TO/AVALANCHE/LIB make

client.cpp
----------

The client listens for incoming event data (PackedRec objects), and reconstructs the original data.

