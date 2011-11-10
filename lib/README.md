Avalanche C++ Library
=====================

AvalancheServer
---------------

=== Creating a Server ===

    AvalancheServer::AvalancheServer(std::string _addr)

Example:

    AvalancheServer* server = new AvalacheServer("tcp://localhost:7777");

This will publish objects on localhost port 7777 via TCP.

=== Sending an Object ===

    int AvalancheServer::sendObject(TObject* o)

Example:

    if (AvalancheServer::sendObject(o) != 0)
        std::cout << "Couldn't send object\n";

This will send the object "o" out to all connected clients.

