#include <zmq.hpp>
#include <iostream>
#include <TH1F.h>
#include <TBuffer.h>
#include <TBufferFile.h>

// build: g++ client.cpp -o client -lzmq -I$ROOTSYS/include `root-config --libs` -g

int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_PULL);
    subscriber.bind("tcp://*:5556");

    zmq::message_t message;
    if (subscriber.recv(&message)) {
        std::cout << "received " << message.size() << " bytes" << std::endl;
        TBufferFile buf(TBuffer::kRead, message.size(), message.data(), false);
	TH1F* h1 = (TH1F*)(buf.ReadObjectAny(TH1F::Class()));
	if (h1)
	    std::cout << h1->GetMean() << std::endl;
	else
	   std::cout << "sadness\n";
    }
    else {
	std::cout << "boo hoo\n";
    }

    return 0;
}

