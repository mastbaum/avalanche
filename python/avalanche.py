'''Python interface to avalanche, wrapped using CINT and PyROOT.

Provides:

    `avalanche.client`:
        A "listener" which may be connected to a dispatcher stream and many
        CouchDB changes feeds

    `avalanche.ratDocObjectMap`:
        A functor which turns JSON documents into RAT::DS TObjects. Converts
        CouchDB changes for insertion into the data stream.

    `avalanche.docObjectMap`:
        Abstract base functor for converting JSON to TObjects. Create one (in
        C++) to listen for record types not handled by `ratDocObjectMap`.

Example Usage:

    import avalanche

    c = avalanche.client()
    map = avalanche.ratDocObjectMap()
    c.addDB('http://localhost:5984', 'dbname', map)

    o = c.recv()

'''

from os.path import abspath, dirname, join
from rat import ROOT
ROOT.gROOT.SetBatch(True)

libpath = join(dirname(dirname(abspath(__file__))), 'lib')

ROOT.gROOT.ProcessLine('.L %s/avalanche_rat.so' % libpath)

client = ROOT.avalanche.client
docObjectMap = ROOT.avalanche.docObjectMap
ratDocObjectMap = ROOT.avalanche.ratDocObjectMap

