#ifndef __AVALANCHE_H__
#define __AVALANCHE_H__

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <pthread.h>
#include <zmq.hpp>
//#include <json_value.h>

#include "http.hpp"

class TObject;

namespace avalanche {

    /** 
     * An avalanche dispatcher server
     *
     * The server serializes ROOT TObjects and sends them out on a ZeroMQ
     * publish socket, to which many clients may be subscribed
     */
    class server
    {
        public:
            /**
             * Create an avalanche server
             * @param _addr The address to bind the server to
             */
            server(std::string _addr);

            /** Destroy this server, closing any open connections */
            ~server() {};

            /**
             * Send a TObject
             * @param o A reference to the object to send. The caller maintains ownership.
             * @return 0 if successful, 1 if unsuccessful
             */
            int sendObject(TObject* o) const;

        protected:
            std::string address;     //< The local server address
            zmq::context_t* context; //< ZeroMQ context for the server socket
            zmq::socket_t* socket;   //< ZeroMQ server socket
    };

    /**
     * An avalanche data stream client
     *
     * The client receives TObjects from both dispatcher streams (i.e.
     * avalanche::servers) and from CouchDB databases, aggregating events from
     * the builder and headers in the database into one homogeneous stream.
     *
     * Clients may connect to unlimited numbers of dispatchers and databases,
     * but data from different sources is not guaranteed to be ordered.
     */
    class client {
        public:
            /** Create a new client */
            client();

            /**
             * Destroy this client, closing any open connections and
             * terminating any running threads
             */
            ~client();

            /**
             * Connect to a dispatcher stream. This may be called repeatedly
             * to "watch" many streams with data interleaved.
             * @param _addr The address of the dispatcher server
             */
            void addDispatcher(std::string _addr);

            /**
             * Connect to a CouchDB database, watching the changes feed for
             * header data. This may be called repeatedly to "watch" many
             * databases.
             * @param _host Hostname of the CouchDB server, optionally
             *              including login information like:
             *              http://user:password@host:port/dbname
             * @param _dbname Name of the database to watch
             * @param _filter Name of the CouchDB filter function to apply to
             *                the changes feed
             */
            void addDB(std::string _host, std::string _dbname, std::string _filterName="");

            /**
             * Get the lists of connected dispatchers and databases
             * @return A map (keys "dispatcher", "couchdb") of lists of
             *         connection identifier strings
             */
            std::map<std::string, std::vector<std::string> > getStreamList();

            /**
             * Receive the next available record
             * @param blocking If true, wait until data is available to return
             * @return The next available TObject
             */
            TObject* recv(bool blocking=false);

        protected:
            std::queue<TObject*> queue;  //< Buffer of received objects
            pthread_mutex_t* queueMutex; //< Mutex protection for queue
            zmq::context_t* context;     //< ZeroMQ context for dispatcher
            zmq::socket_t* socket;       //< ZeroMQ socket for dispatcher
            std::vector<pthread_t*> threads; //< List of all "watcher" threads
            /**
             * A map with lists of connected streams
             * e.g. streams["dispatcher"] -> ["tcp://localhost:5024", ...]
             */
            std::map<std::string, std::vector<std::string> > streams;
    };

    /**
     * Container for dispatcher stream state
     * Used to pass state information to a watchDispatcher thread
     * @see watchDispatcher()
     */
    struct dispatcherState {
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
    struct dbState {
        std::queue<TObject*>* queue;
        pthread_mutex_t* queueMutex;
        std::string host;
        std::string dbname;
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
     * @param arg A reference dispatcherState struct casted to a void*
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
     * @param arg A reference dbState struct casted to a void*
     */
    void* watchDB(void* arg);

    static size_t ptr_to_stream(void* ptr, size_t size, size_t nmemb, void* stream);
    static size_t db_changes_curl_callback(void* ptr, size_t size, size_t nmemb, void* stream);

    /**
     * Convert a JSON document into a RAT::DS::PackedRec
     * @param doc The document to convert to a PackedRec
     * @return The PackedRec representation, or NULL if conversion failed
     */
    //TObject* docToRecord(Json::Value* doc);

} // namespace avalanche

#endif

