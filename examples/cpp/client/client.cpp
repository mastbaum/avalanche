#include <iostream>
#include <TH1F.h>
#include <RAT/DS/PackedEvent.hh>
#include <avalanche.hpp>
#include <avalanche_rat.hpp>

int main(int argc, char* argv[]) {
    // create a client
    avalanche::client client;

    // connect to a few dispatchers
    client.addDispatcher("tcp://localhost:5025");
    client.addDispatcher("tcp://localhost:5024");
    avalanche::docObjectMap map = &(avalanche::docToRecord);
    client.addDB("http://mastbaum:pw123@localhost:5984", "changes_perf", map);

    // receive RAT::DS::PackedRec objects
    while (1) {
        RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client.recv();
        if (rec) {
            std::cout << "Received PackedRec of type " << rec->RecordType << std::endl;
            if (rec->RecordType == 1) {
                RAT::DS::PackedEvent* event = dynamic_cast<RAT::DS::PackedEvent*> (rec->Rec);
                std::cout << " NHIT = " << event->NHits << std::endl;
            }
        }
        else
            continue;
        delete rec;
    }

    return 0;  
}

