#include <zmq.hpp>
#include <TClass.h>
#include <TBuffer.h>
#include <TBufferFile.h>
#include <stdlib.h>
#include <string>

#include "avalanche.hpp"

namespace avalanche {

client::client(std::string _addr) {
    // set up zeromq sockets
    context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    addServer(_addr);
}

client::~client() {
    delete socket;
    delete context;
}

void client::addServer(std::string _addr) {
    serv_addr.push_back(_addr);
    socket->connect(_addr.c_str());
}

void* client::recvObject(TClass* cls, int flags) {
    zmq::message_t message;

    try {
        socket->recv(&message, flags);
        if (message.size() == 0)
            return NULL;

        // TBufferFile used for TObject serialization
        TBufferFile bf(TBuffer::kRead, message.size(), message.data(), false);
        void* bfo = bf.ReadObjectAny(cls);

        // make a copy, since the buffer goes away
        void* o = NULL;
        if (bfo) {
            o = cls->New();
            memcpy(o, bfo, cls->Size());
        }

        return o;
    }
    catch (zmq::error_t e) {
        if (e.num() == EAGAIN) // no data in buffer
            return NULL;
        else
            throw e;
    }
}

} // namespace avalanche

