#include "AvalancheServer.hpp"
#include <TH1F.h>

int main(int argc, char* argv[]) {
    // create a ROOT TObject
    TH1F *h1 = new TH1F("h1","hist",10,0,10);
    h1->Fill(5);

    // ship it
    // (why yes, it's that simple!)
    AvalancheServer* as = new AvalancheServer("tcp://localhost:5024");
    as->sendObject(h1);
}

