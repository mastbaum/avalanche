import couchdb
import ROOT

def doc_to_record(doc):
    '''convert a couchdb header document into a RAT::DS::PackedRec object'''
    # record types
    RHDR = 2
    CAAC = 3
    CAST = 4
    TRIG = 5
    EPED = 6

    o = ROOT.RAT.DS.PackedRec()

    if not 'type' in doc:
        return None

    if doc['type'] == 'rhdr':
        o.RecordType = RHDR
        r = ROOT.RAT.DS.RHDR()
        r.Date = int(doc['date'])
        r.Time = int(doc['time'])
        r.DAQVer = int(doc['daq_ver'])
        r.CalibTrialID = int(doc['calib_trial_id'])
        r.SrcMask = int(doc['source_mask'])
        r.RunMask = int(doc['run_mask'])
        r.CrateMask = int(doc['crate_mask'])
        r.FirstEventID = int(doc['first_event_id'])
        r.ValidEventID = int(doc['valid_event_id'])
        r.RunID = int(doc['run_id'])

    elif doc['type'] == 'caac':
        o.RecordType = CAAC
        r = ROOT.RAT.DS.CAAC()
        for i in range(3):
            r.AVPos[i] = float(doc['av_pos'][i])
            r.AVRoll[i] = float(doc['av_roll'][i])
        for i in range(7):
            r.AVRopeLength[i] = float(doc['av_rope_length'][i])

    elif doc['type'] == 'cast':
        o.RecordType = CAST
        r = ROOT.RAT.DS.CAST()
        r.SourceID = int(doc['source_id'])
        r.SourceStat = int(doc['source_stat'])
        r.NRopes = int(doc['nropes'])
        r.SourcePosUncert1 = float(doc['src_pos_uncert1'])
        r.LBallOrient = float(doc['lball_orient'])
        for i in range(3):
            r.ManipPos[i] = float(doc['manip_pos'][i])
            r.ManipDest[i] = float(doc['manip_dest'][i])
            r.SrcPosUncert2[i] = float(doc['src_pos_uncert2'][i])
        for i in range(len(doc['rope_id'])):
            r.RopeID.push_back(int(doc['rope_id'][i]))
            r.RopeLen.push_back(float(doc['rope_len'][i]))
            r.RopeTargLen.push_back(float(doc['rope_targ_len'][i]))
            r.RopeVel.push_back(float(doc['rope_vel'][i]))
            r.RopeTens.push_back(float(doc['rope_tens'][i]))
            r.RopeErr.push_back(float(doc['rope_err'][i]))

    elif doc['type'] == 'trig':
        o.RecordType = TRIG
        r = ROOT.RAT.DS.TRIG()
        r.TrigMask = int(doc['trigmask'])
        for i in range(10):
            r.Threshold[i] = int(doc['threshold'][i])
            r.TrigZeroOffset[i] = int(doc['trig_zero_offset'][i])
        r.PulserRate = int(doc['pulser_rate'])
        r.MTC_CSR = int(doc['mtc_csr'])
        r.LockoutWidth = int(doc['lockout_width'])
        r.PrescaleFreq = int(doc['prescale_freq'])
        r.EventID = int(doc['event_id'])
        r.RunID = int(doc['run_id'])

    elif doc['type'] == 'eped':
        o.RecordType = EPED
        r = ROOT.RAT.DS.EPED()
        r.GTDelayCoarse = int(doc['gtdelay_coarse'])
        r.GTDelayFine = int(doc['gtdelay_fine'])
        r.QPedAmp = int(doc['qped_amp'])
        r.QPedWidth = int(doc['qped_width'])
        r.PatternID = int(doc['pattern_id'])
        r.CalType = int(doc['caltype'])
        r.EventID = int(doc['event_id'])
        r.RunID = int(doc['run_id'])

    else:
        # unknown record type
        return None

    o.Rec = r
    return o

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
        - filter: string, *optional*
            Name of the couchdb filter function to apply to the changes feed
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
                    rec = doc_to_record(doc)
                    if rec:
                        self.queue.put(rec)
                except couchdb.http.ResourceNotFound:
                    pass

