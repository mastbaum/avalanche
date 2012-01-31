import couchdb
import ROOT

class CouchDB:
    '''A connection to a couchdb server's changes feed.

    Parameters:
        - queue: Queue.Queue
            Queue into which received data is put
        - host: string
            The hostname (with port) of the couchdb server,
            e.g. 'http://localhost:5984'
        - dbname: string
            The name of the database to watch, e.g. 'mydb'
        - username: string, *optional*
            Username for authentication to the database
        - password: string, *optional*
            Password for authentication to the database
    '''
    def __init__(self, queue, host, dbname, filter=None, username=None, password=None):
        self.queue = queue
        couch = couchdb.Server(host)
        couch.resource.credentials = (username, password)
        if couch.version() < '1.1.0':
            raise RuntimeError('couchdb version >= 1.1.0 required')
        self.db = couch[dbname]

        self.kill = False
        self.seq = 0
        self.filter = filter

    def stop(self):
        self.kill = True

    def run(self):
        '''receive data from the database's changes feed forever'''
        while True:
            if self.kill:
                break
            self.recv()

    def recv(self):
        '''listens to the changes feed of the database and yields changed
        documents. optionally limit results using a couchdb filter function
        named by filter, and providing a starting sequence number via seq.
        '''
        changes = self.db.changes(since=self.seq, filter=self.filter)
        self.seq = changes['last_seq']
        results = changes['results']

        for row in results:
            if 'id' in row:
                try:
                    self.queue.put(self.db[row['id']])
                except couchdb.http.ResourceNotFound:
                    pass

