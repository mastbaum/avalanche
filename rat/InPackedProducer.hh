////////////////////////////////////////////////////////////////////
// Last svn revision: $Id: UnPackEvents.hh 628 2011-10-17 16:53:43Z orebi $
////////////////////////////////////////////////////////////////////
/// \class RAT::InPackedProducer
///
/// \brief  Unpacker, to unpack the packed data format into the RAT DS
///          
/// \author Gabriel D. Orebi Gann <orebi@hep.upenn.edu>
///
/// REVISION HISTORY:\n
///     09 Apr 2010 : Gabriel Orebi Gann - extend routines to
///                   unpack header info, as well as detector events,
///                   and various consistency checks (e.g. record 
///                   ordering, PMTBundle eventID matches event-level
///                   eventID etc)
///     28 May 2010 : Gabriel Orebi Gann - flip final bit of PMT Q,T
///                   values (the ADC outputs them flipped)
///     03 Mar 2011 : Gabriel Orebi Gann - edits to incorporate new channel 
///                   types
///     17 Oct 2011 : Gabriel Orebi Gann - Move bit funcs to BitManip
///     19 Dec 2011 : A Mastbaum - factor unpacking functions out so that
///                   they can be reused elsewhere (e.g. dispatcher reader)
///
///  \detail  This class unpacks files from the packed data format
///           that is output by the event builder, and populates the
///           full RAT DS.  This can be run on either real data, or
///           on packed MC files.
///
////////////////////////////////////////////////////////////////////

#ifndef __RAT_InPackedProducer__
#define __RAT_InPackedProducer__

#include <RAT/Producer.hh>
#include <globals.hh>

class G4UIcmdWithAString;

namespace RAT {

class InPackedProducer : public Producer {
public:
  InPackedProducer(ProcBlock* block);
  virtual ~InPackedProducer() {};

  virtual bool ReadEvents(G4String filename);
  virtual void SetNewValue(G4UIcommand* command, G4String newValue);

protected:
  G4UIcmdWithAString* fReadCmd;
};

} // namespace RAT

#endif
