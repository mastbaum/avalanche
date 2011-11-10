#include <zmq.hpp>
#include <time.h>
#include <iostream>
#include <TH1F.h>
#include <TBuffer.h>
#include <TBufferFile.h>

// build: g++ client.cpp -o client -lzmq -I$ROOTSYS/include `root-config --libs` -g

int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0); //filter, strlen (filter));
    subscriber.bind("tcp://*:5024");
    int count=0;

    while (1) {
        zmq::message_t message;
        subscriber.recv(&message);
	std::cout << count << " ";
	count++;
        TBufferFile buf(TBuffer::kRead, message.size(), message.data(), false);
	TH1F* h1 = (TH1F*)(buf.ReadObjectAny(TH1F::Class()));
	if (h1)
	    std::cout << h1->GetMean() << std::endl;
	else
	   std::cout << "sadness\n";
	delete h1;
    }

    return 0;
}

