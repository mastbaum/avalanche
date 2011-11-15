#include <zmq.hpp>
#include <time.h>
#include <iostream>

#include <RAT/DS/Root.hh>
#include <RAT/DS/PackedEvent.hh>
#include <TBuffer.h>
#include <TBufferFile.h>

// build: g++ client.cpp -o client -lzmq -I$ROOTSYS/include `root-config --libs` -g

int main(int argc, char *argv[])
{
    // make a zeromq socket
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0); //filter, strlen (filter));
    subscriber.connect("tcp://*:5024");

    while (1) {
        // listen for incoming messages
        zmq::message_t message;
        subscriber.recv(&message);

	// read message data into a TBufferFile and deserialize
        TBufferFile buf(TBuffer::kRead, message.size(), message.data(), false);
	RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*)(buf.ReadObjectAny(RAT::DS::PackedRec::Class()));
	if (rec)
	    std::cout << "Received PackedRec of type " << rec->RecordType << std::endl;
	else
	   std::cout << "Error deserializing message data" << std::endl;
	delete rec;
    }

    return 0;
}

