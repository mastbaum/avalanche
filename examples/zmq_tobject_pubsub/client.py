import sys
import array
import zmq
import ROOT

if __name__ == '__main__':
    if len(sys.argv) > 1:
        address = sys.argv[1]
    else:
        address = 'tcp://*:5024'

    # set up zeromq socket
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, '')
    socket.bind(address)

    while True:
        msg = socket.recv(copy=False)
        b = array.array('c', msg.bytes)
        buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)
        h = buf.ReadObject(ROOT.TH1F.Class())

        if h:
            print 'Received TH1F with mean', h.GetMean()
        else:
            print 'Error deserializing message data'

