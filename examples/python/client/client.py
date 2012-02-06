#!/usr/bin/env python
import avalanche
import avalanche.ratdb
import signal
import sys
from rat import ROOT

def signal_handler(signal, frame):
    print 'Caught Ctrl-C, exiting...'
    sys.exit(0)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)

    # create avalanche client
    client = avalanche.Client()

    # connect client to a dispatcher stream localhost port 5024
    client.add_dispatcher('tcp://localhost:5024')

    # connect client to a couchdb server at localhost:5984/dispatch
    doc_object_map = avalanche.ratdb.doc_to_record
    client.add_db('http://localhost:5984', 'dbname', doc_object_map,
                  username='username', password='password')

    # receive RAT::DS::PackedRec objects
    while True:
        rec = client.recv()
        if rec is not None:
            print 'Received PackedRec of type', rec.RecordType

