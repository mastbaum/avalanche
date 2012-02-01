#include <zmq.hpp>
#include <TBuffer.h>
#include <TBufferFile.h>
#include <stdlib.h>
#include <string>

#include "avalanche.hpp"

namespace avalanche {

server::server(std::string _addr) {
    // set up zeromq sockets
    address = _addr;
    context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_PUB);
    socket->bind(address.c_str());
}

int server::sendObject(TObject* o) const {
    // use TBufferFile for TObject serialization
    TBufferFile bf(TBuffer::kWrite);
    bf.Reset();

    // ensure object is serialized correctly
    if (bf.WriteObjectAny(o, o->Class()) != 1)
        return 1;

    // zmq_send() frees the memory the message points to, which in our case is
    // owned by a TBufferFile. hence we need to make a copy.
    int bufferSize = bf.Length();
    char* buffer = (char*) malloc(bufferSize * sizeof(char));
    memcpy(buffer, bf.Buffer(), bufferSize);

    // send
    zmq::message_t message((void*)buffer, bufferSize, 0, 0);
    socket->send(message);

    return 0;
}

} // namespace avalanche

