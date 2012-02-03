import couchdb

class CouchDB:
    '''A connection to a couchdb server's changes feed. Changed documents are
    placed in a queue.

    Parameters:
        - queue: Queue.Queue
            Queue into which received data is put
        - host: string
            The hostname (with port) of the couchdb server,
            e.g. 'http://localhost:5984'
        - dbname: string
            The name of the database to watch, e.g. 'mydb'
        - map_function: callable
            A function (or callable) which takes a dictionary as an argument
            and returns a TObject.
        - filter: string, *optional*
            Name of the couchdb filter function to apply to the changes feed
        - username: string, *optional*
            Username for authentication to the database
        - password: string, *optional*
            Password for authentication to the database
    '''
    def __init__(self, queue, host, dbname, map_function, filter=None, username=None, password=None):
        self.queue = queue
        couch = couchdb.Server(host)
        couch.resource.credentials = (username, password)
        if couch.version() < '1.1.0':
            raise RuntimeError('couchdb version >= 1.1.0 required')
        self.db = couch[dbname]

        self.map_function = map_function
        self.kill = False
        self.seq = self.db.info()['update_seq']
        self.filter = filter

    def stop(self):
        '''terminate the run() loop, waiting for any current call to recv()
        to complete
        '''
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
                    doc = self.db[row['id']]
                    rec = self.map_function(doc)
                    if rec:
                        self.queue.put(rec)
                except couchdb.http.ResourceNotFound:
                    pass

