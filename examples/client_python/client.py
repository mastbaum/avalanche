import sys
import array
import zmq
from rat import ROOT

if __name__ == '__main__':
    if len(sys.argv) > 1:
        address = sys.argv[1]
    else:
        address = 'tcp://*:5024'

    # set up zeromq socket
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, '')
    socket.connect(address)

    while True:
        msg = socket.recv(copy=False)
        # buffer contains null characters, so wrap in array to pass to c
        b = array.array('c', msg.bytes)
        buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)
        rec = buf.ReadObject(ROOT.RAT.DS.PackedRec.Class())

        if rec:
            print 'Received PackedRec of type', rec.RecordType
        else:
            print 'Error deserializing message data'

