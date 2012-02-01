#ifndef __AVALANCHE_H__
#define __AVALANCHE_H__

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <pthread.h>
#include <zmq.hpp>
//#include <json_value.h>

class TObject;

namespace RAT {
    namespace DS {
        class PackedRec;
    } // namespace DS
} // namespace RAT

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
        client() {};
        ~client();
	void addDispatcher(std::string _addr);
        void addDB(std::string _host, std::string _dbname, std::string _filterName, std::string _user, std::string _pass);
        std::map<std::string, std::vector<std::string> > getStreamList();
	TObject* recv(bool blocking=false);

    protected:
        std::queue<TObject*> queue;
        zmq::context_t* context;
        zmq::socket_t* socket;
        std::vector<pthread_t*> threads;
        std::map<std::string, std::vector<std::string> > streams;
};

struct dispatcherState {
    std::queue<TObject*>* queue;
    zmq::socket_t* socket;
    int flags;
};

struct dbState {
    std::queue<TObject*>* queue;
    std::string host;
    std::string dbname;
    std::string filterName;
    std::string username;
    std::string password;
};

void* watchDispatcher(void* arg);
void* watchDB(void* arg);

//RAT::DS::PackedRec* docToRecord(Json::Value* doc);

} // namespace avalanche

#endif

