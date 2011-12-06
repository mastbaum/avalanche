#include <iostream>
#include <TH1F.h>
#include <avalanche.hpp>

int main(int argc, char* argv[]) {
  // create a server publishing objects on port 5024
  avalanche::server serv("tcp://*:5024");

  // create and send a TH1F
  TH1F* h = new TH1F("h","h",10,0,10);
  h->Fill(5);
  serv.sendObject(h);

  return 0;  
}

