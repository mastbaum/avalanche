////////////////////////////////////////////////////////////////////
/// \class RAT::DispatchEvents
///
/// \brief Dispatcher, to send MC events out on the network
///          
/// \author A Mastbaum <mastbaum@hep.upenn.edu>
///
/// REVISION HISTORY:\n
///     11 Nov 2011 : A Mastbaum - first revision
///
///  \detail
///
////////////////////////////////////////////////////////////////////
#ifndef __RAT_DispatchEvents___
#define __RAT_DispatchEvents___

#include <RAT/Processor.hh>
#include <RAT/avalanche.hpp>

namespace RAT {

class DispatchEvents : public Processor {
public:
  DispatchEvents();
  virtual ~DispatchEvents();
  virtual Processor::Result DSEvent(DS::Root* ds);

  // address - string, name of the socket for output
  // e.g. "tcp://localhost:7777"
  virtual void SetS(std::string param, std::string value);
  
protected:
  bool fSentRunHeaders;
  std::string address;
  avalanche::server* serv;
};

} // namespace RAT

#endif

