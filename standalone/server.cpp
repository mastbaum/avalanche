#include <zmq.hpp>
#include <iostream>
#include <TH1F.h>
#include <TBuffer.h>
#include <TBufferFile.h>
#include <time.h>
#include <stdlib.h>

// build: g++ server.cpp -o server -lzmq -I$ROOTSYS/include `root-config --libs` -g

int main(int argc, char* argv[])
{
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.connect("tcp://localhost:5024");

    TH1F* h1 = new TH1F("h1", "A histogram", 100, -5, 5);
    h1->FillRandom("gaus", 10);
    std::cout << h1->GetMean() << std::endl;

    for (int i=0; i<100; i++) {
        TBufferFile bf(TBuffer::kWrite);
        bf.Reset();
        if (bf.WriteObjectAny(h1, h1->Class()) != 1) {
            std::cout << "oh noes!" << std::endl;
	    return 1;
        }
        int bufsize = bf.Length();
	char* buf = (char*) malloc(bufsize * sizeof(char));
        memcpy(buf, bf.Buffer(), bufsize);
        zmq::message_t message((void*)buf, bufsize, 0, 0);
        publisher.send(message);
    }

    return 0;
}

