#include <zmq.hpp>
#include <TClass.h>
#include <TBuffer.h>
#include <TBufferFile.h>
#include <stdlib.h>
#include <string>

#include "avalanche.hpp"

namespace avalanche {

client::~client() {
    for (size_t i=0; i<threads.size(); i++) {
        pthread_join(*threads[i], NULL);
        delete threads[i];
    }

    while (!queue.empty()) {
        queue.pop();
    }

    delete context;
}

void client::addDispatcher(std::string _addr) {
    if (!socket) {
        // set up zeromq sockets
        context = new zmq::context_t(1);
        socket = new zmq::socket_t(*context, ZMQ_SUB);
        socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
        streams["dispatcher"].push_back(_addr);

        // launch dispatcher thread with (shared) state
        dispatcherState s;
        s.queue = &queue;
        s.socket = socket;
        s.flags = ZMQ_NOBLOCK;

        pthread_t* dispatcherThread;
        pthread_create(dispatcherThread, NULL, watchDispatcher, (void*) &s);
        threads.push_back(dispatcherThread);
    }

    // add a connection to the socket
    socket->connect(_addr.c_str());
}

void client::addDB(std::string _host, std::string _dbname, std::string _filterName, std::string _user, std::string _pass) {
    streams["couchdb"].push_back(_host + "/" + _dbname);

    // launch couchdb-watching thread with given state
    dbState s;
    s.queue = &queue;
    s.host = _host;
    s.dbname = _dbname;
    s.filterName = _filterName;
    s.username = _user;
    s.password = _pass;

    pthread_t* couchThread;
    pthread_create(couchThread, NULL, watchDB, (void*) &s);
    threads.push_back(couchThread);
}

TObject* client::recv(bool blocking) {
    while (blocking && queue.size() == 0)
        continue;

    if (queue.size() == 0) {
        return NULL;
    }
    else {
        TObject* o = new TObject(*(queue.front()));
        queue.pop();
        return o;
    }
}

void* watchDispatcher(void* arg) {
    dispatcherState s = *((dispatcherState*) arg);

    while(1) {
        zmq::message_t message;

        try {
            s.socket->recv(&message, s.flags);
            if (message.size() == 0)
                continue;

            // TBufferFile used for TObject serialization
            TBufferFile bf(TBuffer::kRead, message.size(), message.data(), false);
            TObject* o = new TObject(*((TObject*) bf.ReadObjectAny(TObject::Class())));

            s.queue->push(o);
        }
        catch (zmq::error_t e) {
            if (e.num() == EAGAIN) // no data in buffer
                return NULL;
            else
                throw e;
        }
    }

    return NULL;
}

void* watchDB(void* arg) {
    dbState s = *((dbState*) arg);
    return NULL;
}

} // namespace avalanche

