////////////////////////////////////////////////////////////////////
// Last svn revision: $Id: UnPackEvents.cc 628 2011-10-17 16:53:43Z orebi $ 
////////////////////////////////////////////////////////////////////

#include <RAT/InPackedProducer.hh>
#include <RAT/Pack.hh>
#include <RAT/ProcBlock.hh>
#include <RAT/DS/PackedEvent.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/RunStore.hh>
#include <RAT/SignalHandler.hh>
#include <RAT/Log.hh>
#include <RAT/DB.hh>

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>

#include <TChain.h>

namespace RAT {

InPackedProducer::InPackedProducer(ProcBlock* block)
{
  SetMainBlock(block);

  // build commands
  G4UIdirectory* DebugDir = new G4UIdirectory("/rat/unpack/");
  DebugDir->SetGuidance("Read packed-format events from a ROOT file");

  // info message command
  fReadCmd = new G4UIcmdWithAString("/rat/unpack/read", this);
  fReadCmd->SetGuidance("name of packed input file");
  fReadCmd->SetParameterName("filename", false); // required
}

void InPackedProducer::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fReadCmd) {
    if (!ReadEvents(newValue))
      Log::Die("InPackedProducer: Error reading event tree from " + newValue);
  }
  else
     Log::Die("InPackedProducer: Invalid unpack \"set\" command");
}

bool InPackedProducer::ReadEvents(G4String filename)
{
  // set up input tree
  TChain* tree = new TChain("PackT");
  if (!tree->Add(filename.c_str()))
    return false;
  DS::PackedRec* rec = new DS::PackedRec();
  tree->SetBranchAddress("PackRec", &rec);

  // holders for data that perists across many events
  DS::TRIGInfo* currentTRIG = NULL;
  DS::EPEDInfo* currentEPED = NULL;
  DS::Run* currentRun = NULL;
  bool runLevelDataIsSet = false;

  // Loop over packed records
  int irecord = 0;
  while (irecord<tree->GetEntries() && !SignalHandler::IsTermRequested()) {
    tree->GetEntry(irecord);
    detail << "InPackedProducer: Processing record " << irecord << " (type " << rec->RecordType << ")" << newline;
 
    switch (rec->RecordType) {
      // EVENT: unpack DS::PackedEvent into DS::Root, run processor block
      case Pack::RecordType::EVENT: {
        DS::PackedEvent* pev = dynamic_cast<DS::PackedEvent*>(rec->Rec);
        Log::Assert(pev, dformat("InPackedProducer: Encountered corrupted PackedEvent in record %i\n", irecord));

        // some run-level data has to come from a PackedEvent
        if (currentRun && !runLevelDataIsSet) {
          currentRun->SetSubRunID(pev->SubRunID);
          currentRun->SetMCFlag(pev->MCFlag);
          currentRun->SetPackVer(pev->PackVer);
          currentRun->SetDataType(pev->DataType);
        }

        DS::Root* ds = Pack::UnpackEvent(pev, currentTRIG, currentEPED);
        fMainBlock->DSEvent(ds);
        delete ds;
        break;
      }
      // RHDR: unpack DS::RHDR to set up a new DS::Run
      case Pack::RecordType::RHDR: {
        DS::RHDR* rhdr = dynamic_cast<DS::RHDR*>(rec->Rec);
        Log::Assert(rhdr, dformat("InPackedProducer: Encountered corrupted PackedEvent in record %i\n", irecord));
        currentRun = Pack::UnpackRHDR(rhdr);
        DS::RunStore::AddNewRun(currentRun);
        runLevelDataIsSet = false;
        break;
      }
      // CAAC: unpack DS::CAAC into this run's DS::AVStat
      case Pack::RecordType::CAAC: {
        DS::CAAC* caac = dynamic_cast<DS::CAAC*>(rec->Rec);
        Log::Assert(caac, dformat("InPackedProducer: Encountered corrupted CAAC header in record %i\n", irecord));
        DS::AVStat* avstat = currentRun->GetAVStat();
	DS::AVStat* asUnpacked = Pack::UnpackCAAC(caac);
        *avstat = *asUnpacked;
	delete asUnpacked;
        break;
      }
      // CAST: unpack DS::CAST into this run's DS::ManipStat
      case Pack::RecordType::CAST: {
        DS::CAST* cast = dynamic_cast<DS::CAST*>(rec->Rec);
        Log::Assert(cast, dformat("InPackedProducer: Encountered corrupted CAST header in record %i\n", irecord));
        DS::ManipStat* manipstat = currentRun->GetManipStat();
	DS::ManipStat* msUnpacked = Pack::UnpackCAST(cast);
        *manipstat = *msUnpacked;
	delete msUnpacked;
        break;
      }
      // TRIG: unpack DS::TRIG and set current TRIG header to apply to events
      case Pack::RecordType::TRIG: {
        delete currentTRIG;
        DS::TRIG* trig = dynamic_cast<DS::TRIG*>(rec->Rec);
        Log::Assert(trig, dformat("InPackedProducer: Encountered corrupted TRIG header in record %i\n", irecord));
        currentTRIG = Pack::UnpackTRIG(trig);
        break;
      }
      // EPED: unpack DS::EPED and set current EPED header to apply to events
      case Pack::RecordType::EPED: {
        delete currentEPED;
        DS::EPED* eped = dynamic_cast<DS::EPED*>(rec->Rec);
        Log::Assert(eped, dformat("InPackedProducer: Encountered corrupted EPED header in record %i\n", irecord));
        currentEPED = Pack::UnpackEPED(eped);
        break;
      }
      // default: unknown record type, file is probably corrupted
      default:
        Log::Die(dformat("InPackedProducer: Invalid record type %i for record %i\n", rec->RecordType, irecord));
    }

    irecord++;
  }

  return true;
}

} // namespace RAT

