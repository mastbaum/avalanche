#include <zmq.hpp>
#include <pthread.h>
#include <string>
#include <RAT/DS/PackedEvent.hh>

#include "avalanche.hpp"

namespace avalanche {

    client::client() : context(NULL), socket(NULL) {
        queueMutex = new pthread_mutex_t();
        pthread_mutex_init(queueMutex, NULL);
    }

    client::~client() {
        // destroy threads
        for (size_t i=0; i<threads.size(); i++) {
            pthread_join(*threads[i], NULL);
            delete threads[i];
        }

        // clean up queue
        pthread_mutex_unlock(queueMutex);
        pthread_mutex_destroy(queueMutex);
        delete queueMutex;

        while (!queue.empty()) {
            queue.pop();
        }

        // clean up zeromq
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
            dispatcherState s;
            s.queue = &queue;
            s.queueMutex = queueMutex;
            s.socket = socket;
            s.flags = ZMQ_NOBLOCK;

            pthread_t* dispatcherThread = new pthread_t();
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
        s.queueMutex = queueMutex;
        s.host = _host;
        s.dbname = _dbname;
        s.filterName = _filterName;
        s.username = _user;
        s.password = _pass;

        pthread_t* couchThread = new pthread_t();
        pthread_create(couchThread, NULL, watchDB, (void*) &s);
        threads.push_back(couchThread);
    }

    RAT::DS::PackedRec* client::recv(bool blocking) {
        while (blocking && queue.empty())
            continue;

        if (queue.empty()) {
            return NULL;
        }
        else {
            pthread_mutex_lock(queueMutex);
            // make a copy of the oldest item; pop() calls destructor 
            RAT::DS::PackedRec* o = new RAT::DS::PackedRec(*(queue.front()));
            queue.pop();
            pthread_mutex_unlock(queueMutex);
            return o;
        }
    }

} // namespace avalanche

