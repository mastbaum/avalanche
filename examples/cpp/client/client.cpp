#include <iostream>
#include <TH1F.h>
#include <RAT/DS/PackedEvent.hh>
#include <avalanche.hpp>

int main(int argc, char* argv[]) {
  // create a client, listening for objects on port 5024
  avalanche::client client("tcp://localhost:5024");

  // receive RAT::DS::PackedRec objects
  while (1) {
    RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client.recvObject(RAT::DS::PackedRec::Class());
    if (rec)
      std::cout << "Received PackedRec of type " << rec->RecordType << std::endl;
    else
      std::cout << "Error deserializing message data" << std::endl;
    delete rec;
  }

  return 0;  
}

