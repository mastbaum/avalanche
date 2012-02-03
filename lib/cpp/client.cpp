#include <zmq.hpp>
#include <pthread.h>
#include <stdint.h>
#include <string>

#include <TObject.h>
#include <TClass.h>

#include "avalanche.hpp"
#include "stream.hpp"

namespace avalanche {

    client::client() : context(NULL), socket(NULL) {
        queueMutex = new pthread_mutex_t();
        pthread_mutex_init(queueMutex, NULL);
    }

    client::~client() {
        // destroy threads, then state
        std::map<pthread_t*, streamState*>::iterator it;
        for (it = threads.begin(); it != threads.end(); ++it) {
            pthread_join(*((*it).first), NULL);
            delete (*it).first;
        }
        for (it = threads.begin(); it != threads.end(); ++it) {
            delete (*it).second;
        }

        // queue cleanup
        pthread_mutex_unlock(queueMutex);
        pthread_mutex_destroy(queueMutex);
        delete queueMutex;

        while (!queue.empty()) {
            queue.pop();
        }

        // zeromq cleanup
        delete context;
        delete socket;
    }

    void client::addDispatcher(std::string _addr) {
        if (!socket) {
            // set up zeromq sockets
            context = new zmq::context_t(1);
            socket = new zmq::socket_t(*context, ZMQ_SUB);
            socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
            streams["dispatcher"].push_back(_addr);

            // launch dispatcher thread with (shared) state
            dispatcherState* s = new dispatcherState();
            s->queue = &queue;
            s->queueMutex = queueMutex;
            s->socket = socket;
            s->flags = ZMQ_NOBLOCK;

            pthread_t* dispatcherThread = new pthread_t();
            pthread_create(dispatcherThread, NULL, watchDispatcher, (void*) s);
            threads[dispatcherThread] = s;
        }

        // add a connection to the socket
        socket->connect(_addr.c_str());
    }

    void client::addDB(std::string _host, std::string _dbname, docObjectMap _map, std::string _filterName) {
        streams["couchdb"].push_back(_host + "/" + _dbname);

        // launch couchdb-watching thread with given state
        dbState* s = new dbState();
        s->queue = &queue;
        s->queueMutex = queueMutex;
        s->host = _host;
        s->dbname = _dbname;
        s->map = _map;
        s->filterName = _filterName;

        pthread_t* couchThread = new pthread_t();
        pthread_create(couchThread, NULL, watchDB, (void*) s);
        threads[couchThread] = s;
    }

    TObject* client::recv(bool blocking) {
        while (blocking && queue.empty())
            continue;

        if (queue.empty()) {
            return NULL;
        }
        else {
            pthread_mutex_lock(queueMutex);
            TObject* o = (TObject*) queue.front()->IsA()->New();
            memcpy(o, queue.front(), o->IsA()->Size());
            queue.pop();
            pthread_mutex_unlock(queueMutex);
            return o;
        }
    }

} // namespace avalanche

