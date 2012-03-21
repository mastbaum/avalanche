/**
 * Proxy device (repeater) for avalanche stream
 *
 * Almost entirely copied from [wuproxy](https://github.com/imatix/zguide/blob/master/examples/C++/wuproxy.cpp)
 * by Olivier Chamoux.
 *
 * Connects to one or many avalanche servers and funnels the aggregate out on
 * another socket. Sockets may be of different types (multicast, tcp).
 *
 * A.Mastbaum <amastbaum@gmail.com>, March 2012
 */

#include <iostream>
#include <stdint.h>
#include <zmq.hpp>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " input [input2 ...] output" << std::endl;
        return 1;
    }

    zmq::context_t context(1);

    // receive on addresses given in argv [1:-1]
    zmq::socket_t frontend(context, ZMQ_SUB);
    frontend.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    for (unsigned i=1; i<argc-1; i++)
        frontend.connect(argv[i]);

    // send on address given in argv [-1]
    zmq::socket_t backend(context, ZMQ_PUB);
    backend.bind(argv[argc-1]);

    // listen forever (to possibly multi-part messages)
    while (true) {
        while (true) {
            zmq::message_t message;
            int64_t more;
            size_t more_size = sizeof(more);

            frontend.recv(&message);
            frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            backend.send(message, more ? ZMQ_SNDMORE : 0);
            if (!more)
                break;
        }
    }

    return 0;
}

