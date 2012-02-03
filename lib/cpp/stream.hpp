#ifndef __AVALANCHE_STREAM_H__
#define __AVALANCHE_STREAM_H__

#include <string>
#include <queue>
#include <pthread.h>

#include "avalanche.hpp"

class TObject;

namespace zmq {
    class socket_t;
}

namespace avalanche {

    /**
     * Container for dispatcher stream state
     * Used to pass state information to a watchDispatcher thread
     * @see watchDispatcher()
     */
    struct dispatcherState : public streamState {
        std::queue<TObject*>* queue;
        pthread_mutex_t* queueMutex;
        zmq::socket_t* socket;
        int flags;
    };

    /**
     * Container for CouchDB stream state
     * Used to pass state information to a watchDB thread
     * @see watchDB()
     */
    struct dbState : public streamState {
        std::queue<TObject*>* queue;
        pthread_mutex_t* queueMutex;
        std::string host;
        std::string dbname;
        docObjectMap map;
        std::string filterName;
    };

    /**
     * Watch a dispatcher stream
     *
     * Listen to a ZeroMQ socket, deserialized TObjects and pushing them into a
     * std::queue as they are received
     *
     * All relevant parameters are passed in via the single argument, which is
     * really a dispatcherState struct. This is necessary because this function
     * is run in a pthread.
     *
     * @param arg A dispatcherState struct* casted to a void*
     */
    void* watchDispatcher(void* arg);

    /**
     * Watch a database stream
     *
     * Listen to a CouchDB changes feed, turning header documents into
     * TObjects and pushing them into a std::queue as they are received
     *
     * All relevant parameters are passed in via the single argument, which is
     * really a dbState struct. This is necessary because this function
     * is run in a pthread.
     *
     * @param arg A dbState struct* casted to a void*
     */
    void* watchDB(void* arg);

    /**
     * Convert a pointer to a byte stream into an std::ostringstream.
     *
     * Used as a callback for receiving generic data with libcurl
     */
    size_t ptr_to_stream(void* ptr, size_t size, size_t nmemb, void* stream);

    /**
     * Parse a byte stream, convert it to a Json::Value, pass it to a function
     * which turns that into a TObject*, and push the TObject* onto the
     * std::queue given in state.
     *
     * Used as a callback for receiving changes feed data with libcurl
     */
    size_t db_change_callback(void* ptr, size_t size, size_t nmemb, void* state);

} // namespace avalanche

#endif

