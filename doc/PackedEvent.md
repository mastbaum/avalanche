Introduction
============
The packed-format data is generally expressed in a stream. Event data is encoded as it comes off the front end and written to disk, sent out to monitoring applications, etc. Various types of data (detector events, run-level headers, and event-level headers) are stored in ROOT TObject subclasses. For convenience of storage these are written to a TTree. Since TTrees are homogeneous lists, various data types must inherit from the same generic superclass to be stored in the tree. The data type must also be stored, so that objects can be properly re-cast when read back.

The TTree stores PackedRec objects, which in turn contain a single GenericRec and an integer RecordType. All data types (events, headers) inherit from GenericRec, which is empty and exists only to support this polymorphism.

The avalanche server broadcasts PackedRec objects, serialized using a ROOT TBufferFile object. Client software must create a TBufferFile using the packet data, read the PackedRec object out of it, and cast the PackedRecs Rec object to the correct type based on the value of RecordType.

The possible classes and their member variables follow.

Packed Data Model
=================
class GenericRec : public TObject
---------------------------------
(empty)

class PackedRec : public TObject
--------------------------------
* UInt_t RecordType
* GenericRec *Rec

### Record Types ###
* 0 - empty
* 1 - detector event
* 2 - RHDR
* 3 - CAAC
* 4 - CAST
* 5 - TRIG
* 6 - EPED

class PackedEvent : public GenericRec
-------------------------------------
* UInt_t MTCInfo[kNheaders] (6 words for the event header from the MTC)
* UInt_t RunID
* UInt_t SubRunID
* UInt_t NHits
* UInt_t EVOrder
* ULong64_t RunMask
* char PackVer
* char MCFlag
* char DataType
* char ClockStat10 
* std::vector<PMTBundle> PMTBundles (vector of PMT bundles)

class PMTBundle 
---------------
* UInt_t Word[3] 

class EPED : public GenericRec
------------------------------
* UInt_t GTDelayCoarse
* UInt_t GTDelayFine
* UInt_t QPedAmp
* UInt_t QPedWidth
* UInt_t PatternID
* UInt_t CalType
* UInt_t EventID (GTID of first events in this bank validity)
* UInt_t RunID (doublecheck on the run)

class TRIG : public GenericRec
------------------------------
* UInt_t TrigMask
* UShort_t Threshold[10]
* UShort_t TrigZeroOffset[10]
* UInt_t PulserRate
* UInt_t MTC_CSR
* UInt_t LockoutWidth
* UInt_t PrescaleFreq
* UInt_t EventID (GTID of first events in this banks validity)
* UInt_t RunID (doublecheck on the run)

### Array Indices ###
Arrays correspond to:
0. N100Lo
1. N100Med
2. N100Hi
3. N20
4. N20LB
5. ESUMLo
6. ESUMHi
7. OWLn
8. OWLELo
9. OWLEHi

class RHDR : public GenericRec
------------------------------
* UInt_t Date
* UInt_t Time
* char DAQVer
* UInt_t CalibTrialID
* UInt_t SrcMask
* UInt_t RunMask
* UInt_t CrateMask
* UInt_t FirstEventID
* UInt_t ValidEventID
* UInt_t RunID (doublecheck on the run)

class CAST : public GenericRec
------------------------------
* UShort_t SourceID
* UShort_t SourceStat
* UShort_t NRopes
* float ManipPos[3]
* float ManipDest[3]
* float SrcPosUncert1
* float SrcPosUncert2[3]
* float LBallOrient
* std::vector<int> RopeID
* std::vector<float> RopeLen
* std::vector<float> RopeTargLen
* std::vector<float> RopeVel
* std::vector<float> RopeTens
* std::vector<float> RopeErr

class CAAC : public GenericRec
------------------------------
* float AVPos[3]
* float AVRoll[3] (roll, pitch and yaw)
* float AVRopeLength[7]

