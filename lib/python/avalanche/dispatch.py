import array
import zmq
import ROOT

class DispatcherStream:
    '''A connection to a dispatcher stream. A Client should only have one
    DispatcherStream, as ZeroMQ handles many connections on one subscribe
    socket.
    
    Parameters:
        - queue: Queue.Queue
            Queue into which objects received are put
        - cls: ROOT.TClass, *optional*
            The TClass of the objects that will be received,
            e.g. ROOT.TH1F.Class()
    '''
    def __init__(self, queue, cls=ROOT.TObject.Class()):
        context = zmq.Context()
        self.socket = context.socket(zmq.SUB)
        self.socket.setsockopt(zmq.SUBSCRIBE, '')
        self.cls = cls
        self.queue = queue
        self.kill = False

    def add_server(self, address):
        '''connect to another stream. clients may listen to an unlimited number
        of streams, the messages from which are interleaved.
        
        Parameters:
            - address: string
                The address of the dispatcher server, e.g. 'tcp://localhost:5024'
        '''
        self.socket.connect(address)

    def set_tclass(self, cls):
        '''set the ROOT TClass received objects are interpreted as'''
        self.cls = cls

    def stop(self):
        self.kill = True

    def run(self):
        '''receive data from the connected streams forever'''
        while True:
            if self.kill:
                break
            self.recv()

    def recv(self, cls=None, flags=zmq.NOBLOCK):
        '''receives and yields a TObject of the class specified by TClass
        `cls`. recv is non-blocking by default, but may be made blocking
        by settings flags to 0.

        Parameters:
            - cls: ROOT.TClass, *optional*
                The TClass of the objects that will be received,
                e.g. ROOT.TH1F.Class()
            - flags: int, *optional*
                Flags passed to the ZeroMQ receive function. Default: NOBLOCK
        '''
        if cls is None:
            cls = self.cls

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
            self.queue.put(o)

