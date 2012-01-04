////////////////////////////////////////////////////////////////////
// Last svn revision: $Id$
////////////////////////////////////////////////////////////////////

#include <RAT/PackEvents.hh>
#include <RAT/Pack.hh>
#include <RAT/Log.hh>
#include <RAT/Processor.hh>
#include <RAT/BitManip.hh>
#include <RAT/DS/PackedEvent.hh>
#include <RAT/DS/RunStore.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/HeaderInfo.hh>

#include <G4UImessenger.hh>
#include <TFile.h>
#include <TTree.h>

namespace RAT {

PackEvents::PackEvents() : Processor("PackEvents")
{
  fEVID = 0;
  fFile = NULL;
  fAutosave = 1024; // kB
  fBranchRec = new DS::PackedRec();
}

PackEvents::~PackEvents()
{
    fFile->cd();
    fTree->Write();
    fFile->Close();

    delete fFile;
}

void PackEvents::SetS(std::string param, std::string value)
{
  if (param == "file") {
    info << "PackEvents: Writing to " << value << newline;
    fFile = TFile::Open(value.c_str(), "recreate");
    Log::Assert(fFile, "PackEvents: Unable to open output file");

    // Set up tree
    fTree = new TTree("PackT", "RAT Tree");	
    fTree->Branch("PackRec", fBranchRec->ClassName(), &fBranchRec, 32000, 99);

    // Allow tree to be recovered up to last N kilobytes if RAT terminates suddenly
    fTree->SetAutoSave(fAutosave*1024);
  }
}

void PackEvents::SetI(std::string param, int value)
{
  if (param == "autosave") {
    Log::Assert(value > 0, "PackEvents: autosave parameter must be a postive integer");
      
    fAutosave = value;
    if (fTree) // fAutosave might be called after the file is already opened
       fTree->SetAutoSave(fAutosave*1024);
  }
}

Processor::Result PackEvents::DSEvent(DS::Root* ds)
{
  Log::Assert(fFile, "PackEvents: Output file required");
  
  // If we're on the first event, fill the initial header information
  if (fEVID == 0) {
    fBranchRec = Pack::MakeRunHeader(ds);
    fTree->Fill();
    delete fBranchRec;
  
    // If AVStat / ManipStat are filled, make CAAC/CAST headers:
    DS::Run* run = RAT::DS::RunStore::GetRun(ds);

    if (run->ExistManipStat()) {
      fBranchRec = Pack::MakeManipHeader(ds);
      fTree->Fill();
      delete fBranchRec;
    }

    if (run->ExistAVStat()) {
      fBranchRec = Pack::MakeAVHeader(ds);
      fTree->Fill();
      delete fBranchRec;
    }
  }
  
  // If this ds event has event-level info attached, make more headers
  if (ds->ExistHeaderInfo()) {
    if (ds->GetHeaderInfo()->ExistTRIGInfo()) {
      fBranchRec = Pack::MakeTRIGHeader(ds);
      fTree->Fill();
      delete fBranchRec;
    }

    if (ds->GetHeaderInfo()->ExistEPEDInfo()) {
      fBranchRec = Pack::MakeEPEDHeader(ds);
      fTree->Fill();
      delete fBranchRec;
    }
  }

  // Loop over triggered events
  for (int iev=0; iev<ds->GetEVCount(); iev++) {
    fBranchRec = Pack::PackEvent(ds, iev);
    fTree->Fill();
    delete fBranchRec;
  }

  fEVID++;

  return Processor::OK;
}

} // namespace RAT

