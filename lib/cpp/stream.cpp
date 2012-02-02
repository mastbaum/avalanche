#include <iostream>
#include <assert.h>
#include <zmq.hpp>
#include <TBuffer.h>
#include <TBufferFile.h>

#include "json/json.h"
#include "json/reader.h"
#include "json/value.h"

#include <curl/curl.h>  
#include "avalanche.hpp"

namespace avalanche {
    class jsonStreamHandler {
        public:
            jsonStreamHandler(Json::Reader* _reader, std::queue<TObject*>* _queue) : reader(_reader), queue(_queue) {}
            void push(std::string data);
        protected:
            Json::Reader* reader;
            std::queue<TObject*>* queue;
    };


    void* watchDispatcher(void* arg) {
        dispatcherState* s = (dispatcherState*) arg;

        while(1) {
            zmq::message_t message;

            try {
                s->socket->recv(&message, s->flags);
                if (message.size() == 0)
                    continue;

                // TBufferFile used for TObject serialization
                TBufferFile bf(TBuffer::kRead, message.size(), message.data(), false);

                // make a copy, since the buffer will disappear
                TObject* o = (TObject*) bf.ReadObjectAny(TObject::Class());

                pthread_mutex_lock(s->queueMutex);
                s->queue->push(o);
                pthread_mutex_unlock(s->queueMutex);
            }
            catch (zmq::error_t &e) {
                if (e.num() == EAGAIN) // no data in buffer
                    return NULL;
                else
                    throw e;
            }
        }

        return NULL;
    }

    void* watchDB(void* arg) {
        dbState* s = (dbState*) arg;

        // set up curl connection
        CURL* curl;
        curl = curl_easy_init();
        assert(curl);
        std::ostringstream data("");

        // get db info to get current update_seq
        std::string query = s->host + "/" + s->dbname;
        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ptr_to_stream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_perform(curl);
        std::cout << "data: " << data.str() << std::endl;
        Json::Reader* reader = new Json::Reader();
        Json::Value v;
        assert(reader->parse(data.str(), v, false));
        assert(v.isObject());
        int update_seq = v["update_seq"].asInt();
        std::cout << "update_seq: " << update_seq << std::endl;

        std::stringstream update_seq_ss;
        update_seq_ss << update_seq;

        // listen to the changes feed forever
        query = s->host + "/" + s->dbname + "/_changes?feed=continuous&include_docs=true&heartbeat=50000&since=" + update_seq_ss.str();
        if (s->filterName != "") {
            query += ("&filter=" + s->filterName);
        }

        jsonStreamHandler* handler = new jsonStreamHandler(reader, s->queue);

        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, db_changes_curl_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, handler);

        curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        return NULL;
    }

    void jsonStreamHandler::push(std::string data) {
        std::cout << "data: " << data << std::endl;
    }

    static size_t ptr_to_stream(void *ptr, size_t size, size_t nmemb, void *stream) {
        std::ostringstream* doc = static_cast<std::ostringstream*>(stream);
        char* data = static_cast<char*>(ptr);

        for (size_t i=0; i < size*nmemb; i++) {
            (*doc) << data[i];
        }

        return size * nmemb;
    }

    static size_t db_changes_curl_callback(void *ptr, size_t size, size_t nmemb, void *handler) {
        jsonStreamHandler* h = static_cast<jsonStreamHandler*>(handler);

        std::ostringstream doc("");
        ptr_to_stream(ptr, size, nmemb, (void*)&doc);

        h->push(doc.str());

        return size * nmemb;
    }

} // namespace avalanche

