#ifndef __AVALANCHE_HTTP_H__
#define __AVALANCHE_HTTP_H__

#include <string>
#include <sstream>

typedef void CURL;

namespace avalanche {

    /** httpDownloader
     *  Utility class for fetching files from a HTTP/HTTPS server using libcurl

     *  httpDownloader uses libcurl to open a persistent connection with an 
     *  HTTP or HTTPS server to download files.  File contents are returned as
     *  std::string objects.
     *
     *  Adapted slightly from the original by Stan Seibert <stan@mtrr.org>
     */
    class httpDownloader {
        public:
            httpDownloader() : handle(0) { }
            ~httpDownloader();

            /** 
             * Close HTTP connection
             *
             * This object will maintain a persistent connection to the HTTP
             * server after a fetch. Call this method to force a disconnect.
             */
            void disconnect();

            /**
             * Fetch a file
             * @param url File URL
             */
            std::string fetch(const std::string &url);

        protected:
            CURL* handle; //!< cURL pointer used to perform http requests
            std::ostringstream contents; //!< container for received data
    };

} // namespace avalanche

#endif

