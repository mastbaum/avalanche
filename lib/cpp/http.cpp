#include <http.hpp>
#include <assert.h>
#include <curl/curl.h>

namespace avalanche {

    httpDownloader::~httpDownloader() {
        disconnect();
    }

    void httpDownloader::disconnect() {
        if (handle) {
            // curl_easy_cleanup deletes its argument
            curl_easy_cleanup(handle);
        }
        handle = 0;
    }

    /* Callback function for curl */
    static size_t curl_write_func(void *ptr, size_t size, size_t nmemb, void *stream) {
        std::ostringstream* doc = static_cast<std::ostringstream*>(stream);
        char* data = static_cast<char*>(ptr);

        for (size_t i=0; i < size*nmemb; i++)
            (*doc) << data[i];

        return size * nmemb;
    }

    std::string httpDownloader::Fetch(const std::string &url) {
        if (!handle) {
            handle = curl_easy_init();
            assert(handle);
            
            // set curl options
            curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1);
            curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &curl_write_func);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &contents);

            // don't use a proxy for connections to localhost
            curl_easy_setopt(handle, CURLOPT_NOPROXY, "localhost,127.0.0.1");
        }

        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        contents.str(""); // clear contents

        curl_easy_perform(handle);

        return contents.str();
    }

} // namespace avalanche

