#include <iostream>
#include <RAT/DS/Root.hh>
#include <avalanche.hpp>
#include <avalanche_rat.hpp>

int main(int argc, char* argv[]) {
    // create a client
    avalanche::client client;

    // connect to a dispatcher
    client.addDispatcher("localhost");

    // connect to couchdb
    avalanche::ratDocObjectMap map;
    client.addDB("http://localhost:5984", "asdf", map);

    // receive RAT::DS TObjects
    std::cout << "Listening..." << std::endl;
    while (1) {
        TObject* o = client.recv();
        if (o) {
            std::cout << "Received TObject of type " << o->IsA()->GetName() << std::endl;
            if (o->IsA() == RAT::DS::Root::Class()) {
                RAT::DS::Root* ds = dynamic_cast<RAT::DS::Root*>(o);
                std::cout << " NHIT = " << ds->GetEV(0)->GetNhits() << std::endl;
            }
        }

        delete o;
    }

    return 0;  
}

