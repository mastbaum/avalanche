#include <pthread.h>
#include <string>
#include <zdab_dispatch.hpp>

#include <TObject.h>

#include <avalanche.hpp>
#include <stream.hpp>

namespace avalanche {

    client::client() {
        queueMutex = new pthread_mutex_t();
        pthread_mutex_init(queueMutex, NULL);
    }

    client::~client() {
        // destroy threads, then state
        std::map<pthread_t*, streamState*>::iterator it;
        for (it=threads.begin(); it!=threads.end(); it++) {
            pthread_join(*((*it).first), NULL);
            delete (*it).first;
        }
        for (it=threads.begin(); it!=threads.end(); it++) {
            delete (*it).second;
        }

        // queue cleanup
        pthread_mutex_unlock(queueMutex);
        pthread_mutex_destroy(queueMutex);
        delete queueMutex;

        while (!queue.empty()) {
            queue.pop();
        }
    }

    void client::addDispatcher(std::string _addr, std::string _records) {
        // create a new dispatcher connection
        ratzdab::dispatch* dispatcher = new ratzdab::dispatch(_addr, _records);
        streams["dispatcher"].push_back(_addr);

        // launch dispatcher thread with (shared) state
        dispatcherState* s = new dispatcherState;
        s->queue = &queue;
        s->queueMutex = queueMutex;
        s->dispatcher = dispatcher; // dispatcherState takes ownership

        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, watchDispatcher, (void*) s);
        threads[thread] = s;
    }

    void client::addDB(std::string _host, std::string _dbname, docObjectMap& _map, std::string _filterName) {
        streams["couchdb"].push_back(_host + "/" + _dbname);

        // launch couchdb-watching thread with given state
        dbState* s = new dbState;
        s->queue = &queue;
        s->queueMutex = queueMutex;
        s->host = _host;
        s->dbname = _dbname;
        s->map = &_map;
        s->filterName = _filterName;

        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, watchDB, (void*) s);
        threads[thread] = s;
    }

    TObject* client::recv(bool blocking) {
        while (blocking && queue.empty()) {
            continue;
        }

        if (queue.empty()) {
            return NULL;
        }
        else {
            pthread_mutex_lock(queueMutex);
            TObject* o = (queue.front())->Clone();
            queue.pop();
            pthread_mutex_unlock(queueMutex);
            return o;
        }
    }

} // namespace avalanche

