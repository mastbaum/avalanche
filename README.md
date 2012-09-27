Avalanche Dispatcher Library 
============================
As in, a lot of SNO(+) coming your way at alarming speed.

The SNO+ network dispatch client library.

Versions
--------
This is the main version of avalanche, which listens for dispatched records using `libratzdab` and is capable of listening to a CouchDB changes feed.

Other versions, maintained on branches in this repository, include:

* [zmq](https://github.com/mastbaum/avalanche/tree/zmq): Client and server libraries for sending ROOT objects around with ZeroMQ PUB/SUB sockets
* [zmq-couchdb](https://github.com/mastbaum/avalanche/tree/zmq-couchdb): `zmq` features, plus can listen to a CouchDB changes feed

Documentation
-------------
Generate doxygen documentation by running

    cd src && doxygen Doxyfile

HTML output is located at `doc/html/index.html`.

Building
--------
To build the `libavalanche`, run

    make

This will create a shared library `libavalanche.so` which you may link into your application.

Source Code
-----------
Avalanche is open source software; code is available on [github](http://github.com/mastbaum/avalanche).

Issues
------
Report any issues at http://github.com/mastbaum/avalanche/issues.

API Overview
============
`avalanche::client`
-------------------
### Creating a Client ###

    avalanche::client()

Example:

    avalanche::client client;
    c.addDispatcher("localhost");
    c.addDB("http://localhost:5984", "mydb", doc_object_map);

This will connect to a ZDAB dispatcher running on localhost and to the changes feed of a CouchDB database at `http://localhost:5984/mydb`.

#### Talking to CouchDB ####
The avalanche client always returns a `TObject`, so when connecting to a database you must give it a functor that will convert CouchDB documents. This is the `doc_object_map` in the above example.

The functor looks like this:

    class myMap : public avalanche::docObjectMap {
        public:
            TObject* operator()(const Json::Value& v) {
                // ... convert and return a TObject ...
            }
    };

Then, to use this to decode a changes feed:

    myMap map;
    client.addDB("http://localhost:5984", "mydb", map);

For SNO+ users, the mapping function is provided. Include `avalanche_rat.hpp` and use:

    avalanche::ratDocObjectMap map;
    client.addDB("http://localhost:5984", "mydb", map);

### Receiving Objects ###

    void* avalanche::client::recv(bool blocking=false)

Example:

    TObject* o = client->recv();
    if (o->IsA()->Class() == TH1F::Class()) {
        TH1F* h = dynamic_cast<TH1F*>(o);
        std::cout << "Mean: " << h->GetMean() << std::endl;
    }

By default, `recv` is non-blocking; `blocking=true` will cause it to wait until data is available before returning.

`recv` always returns a `TObject*`, which must be cast appropriately, likely using `TObject::IsA()`.

