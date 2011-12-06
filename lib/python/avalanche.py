import array
import zmq
import ROOT

class Client:
    '''a client subscribes to an avalanche server and receives ROOT TObjects'''
    def __init__(self, address):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.socket.setsockopt(zmq.SUBSCRIBE, '')
        self.socket.connect(address)

    def recv_object(self, cls, flags=0):
        '''receives and returns a TObject of the class specified by TClass
        `cls`. recv_object blocks by default, but may be made non-blocking
        with the zmq NOBLOCK flag
        '''
        msg = self.socket.recv(flags=flags, copy=False)
        b = array.array('c', msg.bytes)
        buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)

        o = buf.ReadObject(cls)

        return o

