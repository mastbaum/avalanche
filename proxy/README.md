avalanche proxy
===============
A proxy device (repeater) for avalanche stream(s)

This program connects to one or many avalanche servers and funnels the aggregate out on another socket. Sockets may be of differing types (multicast, tcp, etc.).

Building
--------
This software depends only on ZeroMQ (libzmq and zmq.hpp). To build, run `make`.

Usage
-----
    $ ./proxy input [input2 input3 ...] output

where `input(n)` are addresses to listen to (i.e. sources) and `output` is the address the aggregate stream will be published on.

Example:

    $ ./proxy tcp://host1:5000 "epgm://eth0;host2:6000" tcp://*:5024

Credit
------
This was almost entirely copied from [wuproxy](https://github.com/imatix/zguide/blob/master/examples/C++/wuproxy.cpp) by Olivier Chamoux.

