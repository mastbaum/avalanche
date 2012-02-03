import sys
import signal
import threading
import Queue

import dispatch
import db

class Client:
    '''A Client subscribes to one or more dispatcher streams and couchdb
    databases, and produces a stream of event data and headers.
    
    Clients may listen to an unlimited number of streams, but data from
    different sources is not guaranteed to arrive in order!
    '''
    def __init__(self):
        self.queue = Queue.Queue()
        signal.signal(signal.SIGINT, self.signal_handler)

        self.dispatcher_client = None
        self.couchdb_clients = []

        self.threads = []
        self.streams = {'dispatcher': [], 'couchdb': []}

    def signal_handler(self, signal, frame):
        '''handle termination signals cleanly by stopping and re-joining
        client threads
        '''
        print 'Caught Ctrl-C, avalanche exiting...'
        self.dispatcher_client.stop()
        for c in self.couchdb_clients:
            c.stop()
        for thread in self.threads:
            thread.join()

        sys.exit(0)

    def add_dispatcher(self, address):
        '''connect to a dispatcher stream
        
        Args:
            - address (string)
                The address of the dispatcher stream to connect to, e.g.
                tcp://localhost:5024
        '''
        # create dispatcher client thread if necessary
        if not self.dispatcher_client:
            self.dispatcher_client = dispatch.DispatcherStream(self.queue)
            t = threading.Thread(target=self.dispatcher_client.run)
            t.start()
            self.threads.append(t)

        # add server to dispatcher client
        self.dispatcher_client.add_server(address)

        # add address to the list
        self.streams['dispatcher'].append(address)

    def add_db(self, host, dbname, map_function, username=None, password=None):
        '''connect to a couch database

        Args:
            - host (string)
                Hostname of the CouchDB server
            - dbname (string)
                Name of the database
            - map_function (callable)
                A function (or other callable) which converts database
                documents (dictionary-like objects) into ROOT.TObjects
            - username (string), *optional*
                Username for CouchDB server authentication
            - password (string), *optional*
                Password for CouchDB server authentication
        '''
        # create couchdb client
        c = db.CouchDB(self.queue, host, dbname, map_function, username=username, password=password)
        self.couchdb_clients.append(c)

        # create couchdb client thread
        t = threading.Thread(target=c.run)
        t.start()
        self.threads.append(t)

        # add address to the list
        self.streams['couchdb'].append(host + '/' + dbname)

    def recv(self, blocking=False):
        '''receive the next piece of data from connected data sources
        
        Args:
            - blocking (bool), *optional*
                If true, wait for data to arrive before returning. If false,
                return immediately, returning None if there is no data
                available.
        '''
        try:
            return self.queue.get(blocking)
        except Queue.Empty:
            return None

