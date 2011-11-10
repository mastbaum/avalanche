#include <zmq.hpp>
#include <iostream>
#include <TH1F.h>
#include <TBuffer.h>
#include <TBufferFile.h>

// build: g++ server.cpp -o server -lzmq -I$ROOTSYS/include `root-config --libs` -g

int main(int argc, char* argv[])
{
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUSH);
    publisher.connect("tcp://localhost:5556");

    TH1F* h1 = new TH1F("h1", "A histogram", 100, -5, 5);
    h1->FillRandom("gaus", 1000);
    std::cout << h1->GetMean() << std::endl;

    TBufferFile buf(TBuffer::kWrite);
    buf.Reset();
    if (buf.WriteObjectAny(h1, h1->Class()) != 1) {
        std::cout << "oh noes!" << std::endl;
	return 1;
    }

    zmq::message_t message((void*)(&buf), sizeof(buf), 0, 0);
    publisher.send(message);
    std::cout << "sent " << sizeof(buf) << " bytes" << std::endl;

    return 0;
}

