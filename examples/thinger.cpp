#include <iostream>
#include <TH1F.h>
#include "avalanche.hpp"

int main(int argc, char* argv[]) {
  // create a server publishing objects on port 7777
  avalanche::server serv("tcp://*:7777");

  // create a client, listening for objects on port 7777
  avalanche::client cli("tcp://localhost:7777");

  // create and send a TH1F
  TH1F* h = new TH1F("h","h",10,0,10);
  h->Fill(5);
  serv.sendObject(h);

  // receive the TH1F and print the integral
  TH1F* hh = (TH1F*) cli.recvObject(TH1F::Class());
  std::cout << hh << " " << hh->Integral() << std::endl;

  return 0;  
}

