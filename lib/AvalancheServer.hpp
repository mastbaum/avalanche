#include <string>
#include <zmq.hpp>
#include <TObject.h>

class AvalancheServer
{
    public:
        AvalancheServer(std::string _addr);
        ~AvalancheServer() {};
        int sendObject(TObject* o);

    protected:
        std::string address;
        zmq::context_t* context;
        zmq::socket_t* socket;
};

