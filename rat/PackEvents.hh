////////////////////////////////////////////////////////////////////
// Last svn revision: $Id$
////////////////////////////////////////////////////////////////////
/// \class RAT::PackEvents
///
/// \brief Packer, to pack MC events into the packed root format
///          
/// \author Gabriel D. Orebi Gann <orebi@hep.upenn.edu>
///
/// REVISION HISTORY:\n
///     09 Apr 2010 : Gabriel Orebi Gann - extend packing to include
///                   header info, as well as detector events
///     28 May 2010 : Gabriel Orebi Gann - flip final bit of PMT Q,T
///                   values (to mimic the ADC in data, which outputs them 
///                   flipped)
///     03 Mar 2011 : Gabriel Orebi Gann - edits to incorporate new channel 
///                   types
///     17 Oct 2011 : Gabriel Orebi Gann - Move bit funcs to BitManip
///     11 Nov 2011 : A Mastbaum - refactor so that packing functions can be
///                   reused in other code
///
///  \detail  This class packs the full simulated MC events into
///           the same packed root format that the event builder
///           outputs.  i.e. the result is a file that looks as 
///           close to the real data output by the detector as we
///           can manage.
///           NB since we are mimicking real data, only the EV side
///           of the events is packed, not the MC side.
///
////////////////////////////////////////////////////////////////////
#ifndef __RAT_PackEvents___
#define __RAT_PackEvents___

#include <RAT/Processor.hh>
#include <RAT/DS/PackedEvent.hh>

class TFile;
class TTree;

namespace RAT {

class PackEvents : public Processor {
public:
  PackEvents();
  virtual ~PackEvents();
  virtual Processor::Result DSEvent(DS::Root* ds);

  // file - string, name of file to open for output, file will be erased
  virtual void SetS(std::string param, std::string value);
  
  // autosave - integer, update root file every N kilobytes
  virtual void SetI(std::string param, int value);

protected:
  int fEVID;
  int fAutosave;

  TFile* fFile;
  TTree* fTree;
  DS::PackedRec* fBranchRec;
};

} // namespace RAT

#endif

