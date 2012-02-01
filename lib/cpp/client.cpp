#include <iostream>
#include <zmq.hpp>
#include <pthread.h>
#include <stdint.h>
#include <string>

#include <TObject.h>
#include <TClass.h>

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

        // couchdb cleanup
        delete downloader;
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

        if (!downloader) {
            downloader = new httpDownloader();
        }

        // launch couchdb-watching thread with given state
        dbState s;
        s.queue = &queue;
        s.queueMutex = queueMutex;
        s.downloader = downloader;
        s.host = _host;
        s.dbname = _dbname;
        s.filterName = _filterName;
        s.username = _user;
        s.password = _pass;

        pthread_t* couchThread = new pthread_t();
        pthread_create(couchThread, NULL, watchDB, (void*) &s);
        threads.push_back(couchThread);
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

