#include <iostream>
#include <TObject.h>
#include <TClass.h>
#include <avalanche.hpp>
#include <avalanche_rat.hpp>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " http://example.com:5984 dbname" << std::endl;
        return 1;
    }

    std::string db_host(argv[1]);
    std::string db_name(argv[2]);

    avalanche::client c;
    avalanche::ratDocObjectMap m;

    c.addDB(db_host, db_name, m);

    std::cout << "listening..." << std::endl;
    while (true) {
        TObject* o = c.recv();
        std::cout << "got a " << o->IsA()->GetName() << std::endl;
        o->Dump();
    }

    return 0;
}

