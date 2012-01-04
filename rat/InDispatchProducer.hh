////////////////////////////////////////////////////////////////////
// Last svn revision: $Id: UnPackEvents.hh 628 2011-10-17 16:53:43Z orebi $
////////////////////////////////////////////////////////////////////
/// \class RAT::InDispatchProducer
///
/// \brief Read events from the dispatcher stream into RAT
///          
/// \author A. Mastbaum <mastbaum@hep.upenn.edu>
///
/// REVISION HISTORY:\n
///     19 Dec 2011 : A Mastbaum - initial version
///
///  \detail  This producers reads in packed events from the dispatcher
///           stream, output by the event builder or the RAT
///           DispatchEvents processor, and populates the full RAT DS.
///
///           Unlike the InPackedProducer, this will ignore corrupted
///           records and continue.
///
////////////////////////////////////////////////////////////////////

#ifndef __RAT_InDispatchProducer__
#define __RAT_InDispatchProducer__

#include <RAT/Producer.hh>
#include <globals.hh>

class G4UIcmdWithAString;

namespace RAT {

#ifdef __ZMQ

class InDispatchProducer : public Producer {
public:
  InDispatchProducer(ProcBlock* block);
  virtual ~InDispatchProducer() {};

  virtual bool ReadEvents(G4String address);
  virtual void SetNewValue(G4UIcommand* command, G4String newValue);

protected:
  G4UIcmdWithAString* fReadCmd;
};

#else

class InDispatchProducer : public Producer {
public:
  InDispatchProducer(ProcBlock* block) {};
  virtual ~InDispatchProducer() {};

  virtual bool ReadEvents(G4String address) { Die(); }
  virtual void SetNewValue(G4UIcommand* command, G4String newValue) { Die(); }

  void Die()
  {
    Log::Die("InDispachProducer: ZeroMQ support required.");
  }
};

#endif

} // namespace RAT

#endif
