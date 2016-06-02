#ifndef GENERATION_IHEPMCMERGETOOL_H
#define GENERATION_IHEPMCMERGETOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "HepMC/GenEvent.h"

/** @class IHepMCMergeTool IHepMCMergeTool.h "Generation/IHepMCMergeTool.h"
 *
 *  Abstract interface to merge HepMC Events
 *
 */


class IHepMCMergeTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IHepMCMergeTool, 1, 0);

  /// Turn a vector of signal event and subsequent pileup events into a merged event.
  virtual HepMC::GenEvent* merge(const std::vector<HepMC::GenEvent>& eventVector) = 0;

};

#endif // GENERATION_IHEPMCMERGETOOL_H
