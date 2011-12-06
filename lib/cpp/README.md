Avalanche C++ Library
=====================
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

    avalanche::client(std::string _addr)

Example:

    avalanche::client* client = new avalanche::client("tcp://localhost:7777");

This will connect to an avalanche server running on localhost port 7777 via TCP.

### Receiving Objects ###

    void* avalanche::client::recvObject(int flags=0)

Example:

    TH1F* h = (TH1F*) client->recvObject();

By default, recvObject is blocking; `flags=ZMQ_NOBLOCK` will prevent the client from blocking. Note that `recvObject` returns a `void*` (like TBufferFile::ReadObjectAny), which must be cast appropriately.

