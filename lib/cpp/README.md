Avalanche C++ Library
=====================
Documentation
-------------
Generate doxygen documentation by running

    doxygen Doxyfile

HTML output is located at `doc/html/index.html`.

Building
--------
To build the library, which includes the client and server, run

    make

This will create a shared library `libavalanche.so` which you may link into your application.

avalanche::server
-----------------
### Creating a Server ###

    avalanche::server(std::string _addr)

Example:

    avalanche::server* server = new avalanche::server("tcp://localhost:7777");

This will publish objects on localhost port 7777 via TCP.

### Sending an Object ###

    int avalanche::server::sendObject(TObject* o)

Example:

    if (server->sendObject(o) != 0)
        std::cout << "Couldn't send object\n";

This will send the object "o" out to all connected clients.

avalanche::client
-----------------
### Creating a Client ###

    avalanche::client()

Example:

    avalanche::client* client = new avalanche::client();
    client->addDispatcher("tcp://localhost:7777");
    client->addDB("http://localhost:5984", "mydb");

This will connect to an avalanche server running on localhost port 7777 via TCP and the changes feed of a CouchDB database at `http://localhost:5984/mydb`.

### Receiving Objects ###

    void* avalanche::client::recv(bool blocking=false)

Example:

    TH1F* h = (TH1F*) client->recv();

By default, recvObject is non-blocking; `blocking=true` will cause recv() to wait until data is available before returning. Note that `recv` returns a `TObject*`, which must be cast appropriately, likely using `TObject::IsA()`.

