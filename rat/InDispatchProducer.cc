////////////////////////////////////////////////////////////////////
// Last svn revision: $Id: UnPackEvents.cc 628 2011-10-17 16:53:43Z orebi $ 
////////////////////////////////////////////////////////////////////

#ifdef __ZMQ
#include <RAT/InDispatchProducer.hh>
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

#include <RAT/avalanche.hpp>

namespace RAT {

InDispatchProducer::InDispatchProducer(ProcBlock* block)
{
  SetMainBlock(block);

  // build commands
  G4UIdirectory* DebugDir = new G4UIdirectory("/rat/readdispatch/");
  DebugDir->SetGuidance("Read events from the dispatcher stream");

  // info message command
  fReadCmd = new G4UIcmdWithAString("/rat/readdispatch/address", this);
  fReadCmd->SetGuidance("address of dispatcher server");
  fReadCmd->SetParameterName("address", false); // required
}

void InDispatchProducer::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fReadCmd) {
    if (!ReadEvents(newValue))
      Log::Die("InDispatchProducer: Error reading events from server " + newValue);
  }
  else
     Log::Die("InDispatchProducer: Invalid readdispatch \"set\" command");
}

bool InDispatchProducer::ReadEvents(G4String address)
{
  // set up client connection
  avalanche::client* client = new avalanche::client(address);
  info << "InDispatcherProducer: Listening to dispatcher at " << address << newline;

  // holders for data that perists across many events
  DS::TRIGInfo* currentTRIG = NULL;
  DS::EPEDInfo* currentEPED = NULL;
  DS::Run* currentRun = NULL;
  bool runLevelDataIsSet = false;

  // process records as they are received
  while (!SignalHandler::IsTermRequested()) {
    try {
      RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client->recvObject(RAT::DS::PackedRec::Class());

      detail << "InDispatchProducer: Received record of type " << rec->RecordType << newline;
 
      switch (rec->RecordType) {
        // EVENT: unpack DS::PackedEvent into DS::Root, run processor block
        case Pack::RecordType::EVENT: {
          DS::PackedEvent* pev = dynamic_cast<DS::PackedEvent*>(rec->Rec);
          if (!pev) {
            warn << "InDispatchProducer: Encountered corrupted PackedEvent" << newline;
            continue;
          }

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
          if (!rhdr) {
            warn << "InDispatchProducer: Encountered corrupted RHDR header" << newline;
            continue;
          }
          currentRun = Pack::UnpackRHDR(rhdr);
          DS::RunStore::AddNewRun(currentRun);
          runLevelDataIsSet = false;
          break;
        }
        // CAAC: unpack DS::CAAC into this run's DS::AVStat
        case Pack::RecordType::CAAC: {
          DS::CAAC* caac = dynamic_cast<DS::CAAC*>(rec->Rec);
          if (!caac) {
            warn << "InDispatchProducer: Encountered corrupted CAAC header" << newline;
            continue;
          }
          DS::AVStat* avstat = currentRun->GetAVStat();
	  DS::AVStat* asUnpacked = Pack::UnpackCAAC(caac);
          *avstat = *asUnpacked;
          delete asUnpacked;
          break;
        }
        // CAST: unpack DS::CAST into this run's DS::ManipStat
        case Pack::RecordType::CAST: {
          DS::CAST* cast = dynamic_cast<DS::CAST*>(rec->Rec);
          if (!cast) {
            warn << "InDispatchProducer: Encountered corrupted CAST header" << newline;
            continue;
          }
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
          if (!trig) {
            warn << "InDispatchProducer: Encountered corrupted TRIG header" << newline;
            continue;
          }
          currentTRIG = Pack::UnpackTRIG(trig);
          break;
        }
        // EPED: unpack DS::EPED and set current EPED header to apply to events
        case Pack::RecordType::EPED: {
          delete currentEPED;
          DS::EPED* eped = dynamic_cast<DS::EPED*>(rec->Rec);
          if (!eped) {
            warn << "InDispatchProducer: Encountered corrupted EPED header" << newline;
            continue;
          }
          currentEPED = Pack::UnpackEPED(eped);
          break;
        }
        // default: unknown record type, packet is probably corrupted
        default:
          warn << "InDispatchProducer: Encountered invalid record type " << rec->RecordType << newline;
      }
    }
    catch (zmq::error_t &e) {
      // interrupts during zmq::recv sometimes generate exceptions
      continue;
    }
    catch (...) {
      warn << "InDispatchProducer: Caught signal in recv(), sending SIGINT to main..." << newline;
      SignalHandler::SigInt();
      break;
    }
  }

  return true;
}

} // namespace RAT

#endif

