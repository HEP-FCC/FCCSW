#ifndef GENERATION_IHEPMCMERGETOOL_H
#define GENERATION_IHEPMCMERGETOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "HepMC/GenEvent.h"

/** @class IHepMCMergeTool IHepMCMergeTool.h "Generation/IHepMCMergeTool.h"
 *
 *  Abstract interface to merge HepMC Events
 *
 */

static const InterfaceID IID_IHepMCMergeTool( "IHepMCMergeTool" , 3 , 0 ) ;

class IHepMCMergeTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IHepMCMergeTool ; }

  /// Turn a signal event and a vector of pileup events into a merged event.
  virtual StatusCode merge(HepMC::GenEvent& signalEvent, const std::vector<HepMC::GenEvent>& eventVector) = 0;

};

#endif // GENERATION_IHEPMCMERGETOOL_H
