#include <RAT/DispatchEvents.hh>
#include <RAT/Pack.hh>
#include <RAT/Processor.hh>
#include <RAT/Log.hh>
#include <RAT/DS/PackedEvent.hh>
#include <RAT/DS/RunStore.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/HeaderInfo.hh>

#include <G4UImessenger.hh>

namespace RAT {

DispatchEvents::DispatchEvents() : Processor("DispatchEvents")
{
  serv = 0x0;
  fSentRunHeaders = false;
}

DispatchEvents::~DispatchEvents()
{
  delete serv;
}

void DispatchEvents::SetS(std::string param, std::string value)
{
  if (param == "address") {
    info << "DispatchEvents: Publishing to address " << value << newline;
    serv = new AvalancheServer(value);
    Log::Assert(serv, "DispatchEvents: Unable to open socket");
  }
}

Processor::Result DispatchEvents::DSEvent(DS::Root *ds)
{
  Log::Assert(serv, "DispatchEvents: Output address required");
  DS::PackedRec* rec;

  // If we're on the first event, fill the initial header information
  if (!fSentRunHeaders) {
    rec = Pack::MakeRunHeader(ds);
    serv->sendObject(rec);
    delete rec;
  
    // If AVStat / ManipStat are filled, make CAAC/CAST headers:
    DS::Run* run = RAT::DS::RunStore::GetRun(ds);

    if (run->ExistManipStat()) {
      rec = Pack::MakeManipHeader(ds);
      serv->sendObject(rec);
      delete rec;
    }

    if (run->ExistAVStat()) {
      rec = Pack::MakeAVHeader(ds);
      serv->sendObject(rec);
      delete rec;
    }

    fSentRunHeaders = true;
  }
  
  // If this ds event has event-level info attached, make more headers
  if (ds->ExistHeaderInfo()) {
    if (ds->GetHeaderInfo()->ExistTRIGInfo()) {
      rec = Pack::MakeTRIGHeader(ds);
      serv->sendObject(rec);
      delete rec;
    }

    if (ds->GetHeaderInfo()->ExistEPEDInfo()) {
      rec = Pack::MakeEPEDHeader(ds);
      serv->sendObject(rec);
      delete rec;
    }
  }

  // Loop over triggered events
  for (int iev=0; iev<ds->GetEVCount(); iev++) {
    rec = Pack::PackEvent(ds, iev);
    serv->sendObject(rec);
    delete rec;
  }

  return Processor::OK;
}

} // namespace RAT

