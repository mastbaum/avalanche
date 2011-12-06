#!/usr/bin/env python
import avalanche
from rat import ROOT

if __name__ == '__main__':
    # create client listening to localhost port 5024
    cli = avalanche.Client('tcp://localhost:5024')

    # receive RAT::DS::PackedRec objects
    while True:
        rec = cli.recv_object(ROOT.RAT.DS.PackedRec.Class())

        if rec:
            print 'Received PackedRec of type', rec.RecordType
        else:
            print 'Error deserializing message data'

