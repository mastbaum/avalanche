#include <sstream>
#include <assert.h>
#include <curl/curl.h>  
#include <zdab_convert.hpp>
#include <zdab_dispatch.hpp>

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

#include <avalanche.hpp>
#include <stream.hpp>

namespace avalanche {

    void* watchDispatcher(void* arg) {
        dispatcherState* s = (dispatcherState*) arg;

        while(true) {
            try {
                TObject* o = s->dispatcher->next(); // blocks by default

                pthread_mutex_lock(s->queueMutex);
                s->queue->push(o);
                pthread_mutex_unlock(s->queueMutex);
            }
            catch (ratzdab::unknown_record_error& e) {
                std::cerr << "Skipping record of unknown type" << std::endl;
                return NULL;
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

        Json::Reader* reader = new Json::Reader();
        Json::Value v;
        assert(reader->parse(data.str(), v, false));
        assert(v.isObject());
        int update_seq = v["update_seq"].asInt();

        std::stringstream update_seq_ss;
        update_seq_ss << update_seq;

        // listen to the changes feed forever
        query = s->host + "/" + s->dbname + "/_changes?feed=continuous&include_docs=true&heartbeat=50000&since=" + update_seq_ss.str();
        if (s->filterName != "") {
            query += ("&filter=" + s->filterName);
        }

        curl_easy_setopt(curl, CURLOPT_URL, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, db_change_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);

        // start request
        curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        return NULL;
    }

    size_t ptr_to_stream(void* ptr, size_t size, size_t nmemb, void* stream) {
        std::ostringstream* doc = static_cast<std::ostringstream*>(stream);
        char* data = static_cast<char*>(ptr);

        for (size_t i=0; i < size*nmemb; i++)
            (*doc) << data[i];

        return size * nmemb;
    }

    size_t db_change_callback(void* ptr, size_t size, size_t nmemb, void* state) {
        dbState* s = (dbState*) state;

        std::ostringstream doc("");
        ptr_to_stream(ptr, size, nmemb, (void*)&doc);

        if (doc.str().find("{") != std::string::npos) {
            Json::Reader reader;
            Json::Value v;
            reader.parse(doc.str(), v, false);

            if (v.isObject()) {
                TObject* o = (*(s->map))(v["doc"]);
                if (o) {
                    pthread_mutex_lock(s->queueMutex);
                    s->queue->push(o);
                    pthread_mutex_unlock(s->queueMutex);
                }
            }
        }

        return size * nmemb;
    }

} // namespace avalanche

