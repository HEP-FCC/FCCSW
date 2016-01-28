#ifndef GENERATION_IHEPMCMERGETOOL_H
#define GENERATION_IHEPMCMERGETOOL_H 1

#include "GaudiKernel/IAlgTool.h"

#include "HepMC/GenEvent.h"

/** @class IHepMCMergeTool IHepMCMergeTool.h "IHepMCMergeTool.h"
 * 
 *  Abstract interface to merge HepMC Events
 *
 */

static const InterfaceID IID_IHepMCMergeTool( "IHepMCMergeTool" , 3 , 0 ) ;

class IHepMCMergeTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IHepMCMergeTool ; }

  virtual HepMC::GenEvent* merge(const std::vector<HepMC::GenEvent>& eventVector) = 0;

};

#endif // GENERATION_IHEPMCMERGETOOL_H
