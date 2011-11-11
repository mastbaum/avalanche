#ifndef __RAT_Pack___
#define __RAT_Pack___

#include <RAT/DS/Root.hh>
#include <RAT/DS/PackedEvent.hh>

namespace RAT {

class Pack {
public:
  Pack() {};
  virtual ~Pack() {};

  static DS::PackedRec* MakeTRIGHeader(DS::Root* ds);
  static DS::PackedRec* MakeEPEDHeader(DS::Root* ds);
  static DS::PackedRec* MakeRunHeader(DS::Root *ds); 
  static DS::PackedRec* MakeAVHeader(DS::Root *ds);
  static DS::PackedRec* MakeManipHeader(DS::Root *ds);
  static DS::PackedRec* MakeEventHeader(DS::Root *ds);
  static DS::PackedRec* PackEvent(DS::Root* ds, int iev);

  static DS::PMTBundle MakePMTBundle(DS::PMTUnCal *pmt, unsigned int gtid);

protected:
  static const int fPackVer = 0;
};

} // namespace RAT

#endif

