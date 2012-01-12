import array
import zmq
import ROOT

class Client:
    '''a client subscribes to an avalanche server and receives ROOT TObjects'''
    def __init__(self, address):
        self.serv_addr = []
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.socket.setsockopt(zmq.SUBSCRIBE, '')
        self.add_server(address)

    def add_server(self, address):
        '''connect to another stream. clients may listen to an unlimited number
        of streams, the messages from which are interleaved.
        '''
        self.serv_addr.append(address)
        self.socket.connect(address)

    def recv_object(self, cls, flags=0):
        '''receives and returns a TObject of the class specified by TClass
        `cls`. recv_object blocks by default, but may be made non-blocking
        with the zmq NOBLOCK flag.
        '''
        try:
            msg = self.socket.recv(flags=flags, copy=False)
        except zmq.ZMQError, e:
            if e.errno == zmq.EAGAIN:
                pass
            else:
                raise
        else:
            b = array.array('c', msg.bytes)
            buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)
    
            o = buf.ReadObject(cls)

            return o

