#include <string>
#include <zmq.hpp>

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
        ~client() {};
	void* recvObject(TClass* cls, int flags=0);

    protected:
        std::string address;
        zmq::context_t* context;
        zmq::socket_t* socket;
};

} // namespace avalanche

