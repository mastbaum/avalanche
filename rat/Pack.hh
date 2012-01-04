#ifndef __RAT_Pack___
#define __RAT_Pack___

#include <RAT/DS/Root.hh>
#include <RAT/DS/PackedEvent.hh>

namespace RAT {

namespace Pack {
  // version number
  const int packVer = 0;

  // record types
  namespace RecordType {
    enum { NONE, EVENT, RHDR, CAAC, CAST, TRIG, EPED };
  }

  // packing functions map full to packed DS
  DS::PackedRec* MakeTRIGHeader(DS::Root* ds);
  DS::PackedRec* MakeEPEDHeader(DS::Root* ds);
  DS::PackedRec* MakeRunHeader(DS::Root* ds); 
  DS::PackedRec* MakeAVHeader(DS::Root* ds);
  DS::PackedRec* MakeManipHeader(DS::Root* ds);
  DS::PackedRec* PackEvent(DS::Root* ds, int iev);
  DS::PMTBundle MakePMTBundle(DS::PMTUnCal* pmt, unsigned int gtid);

  // unpacking functions map packed to full DS
  DS::Root* UnpackEvent(DS::PackedEvent* pev, DS::TRIGInfo* trig, DS::EPEDInfo* eped);
  DS::Run* UnpackRHDR(DS::RHDR* rhdr);
  DS::AVStat* UnpackCAAC(DS::CAAC* caac);
  DS::ManipStat* UnpackCAST(DS::CAST* cast);
  DS::TRIGInfo* UnpackTRIG(DS::TRIG* trig);
  DS::EPEDInfo* UnpackEPED(DS::EPED* eped);
  DS::PMTUnCal* UnpackPMT(DS::PMTBundle* bundle);

} // namespace Pack

} // namespace RAT

#endif

