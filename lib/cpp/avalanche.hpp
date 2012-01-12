#include <string>
#include <vector>
#include <zmq.hpp>

#ifndef __AVALANCHE_H__
#define __AVALANCHE_H__

class TObject;
class TClass;

namespace avalanche {

class server
{
    public:
        server(std::string _addr);
        ~server() {};
        int sendObject(TObject* o);

    protected:
        std::string address;
        zmq::context_t* context;
        zmq::socket_t* socket;
};

class client {
    public:
        client(std::string _addr);
        ~client();
	void addServer(std::string _addr);
	void* recvObject(TClass* cls, int flags=0);

    protected:
        std::vector<std::string> serv_addr;
        zmq::context_t* context;
        zmq::socket_t* socket;
};

} // namespace avalanche

#endif

