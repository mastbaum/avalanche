#include <zmq.hpp>
#include <TBuffer.h>
#include <TBufferFile.h>
#include <RAT/DS/PackedEvent.hh>

#include "avalanche.hpp"

namespace avalanche {

    void* watchDispatcher(void* arg) {
        dispatcherState s = *((dispatcherState*) arg);

        while(1) {
            zmq::message_t message;

            try {
                s.socket->recv(&message, s.flags);
                if (message.size() == 0)
                    continue;

                // TBufferFile used for TObject serialization
                TBufferFile bf(TBuffer::kRead, message.size(), message.data(), false);

                // make a copy, since the buffer will disappear
                RAT::DS::PackedRec* o = new RAT::DS::PackedRec(*((RAT::DS::PackedRec*) bf.ReadObjectAny(RAT::DS::PackedRec::Class())));

                pthread_mutex_lock(s.queueMutex);
                s.queue->push(o);
                pthread_mutex_unlock(s.queueMutex);
            }
            catch (zmq::error_t &e) {
                if (e.num() == EAGAIN) // no data in buffer
                    return NULL;
                else
                    throw e;
            }
        }

        return NULL;
    }

    void* watchDB(void* arg) {
        dbState s = *((dbState*) arg);
        return NULL;
    }

} // namespace avalanche

