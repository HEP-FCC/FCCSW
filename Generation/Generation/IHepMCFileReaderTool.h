#ifndef GENERATION_IHEPMCFILEREADERTOOL_H
#define GENERATION_IHEPMCFILEREADERTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenEvent;
}

/** @class IHepMCFileReaderTool IHepMCFileReaderTool.h "Generation/IHepMCFileReaderTool.h"
 *
 *  Abstract interface to  read files.
 *
 */


static const InterfaceID IID_IHepMCFileReaderTool( "IHepMCFileReaderTool" , 3 , 0 ) ;

class IHepMCFileReaderTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IHepMCFileReaderTool ; }

  virtual StatusCode readNextEvent(HepMC::GenEvent&) = 0;

  virtual StatusCode open(const std::string& filename) = 0;

};

#endif // GENERATION_IHEPMCFILEREADERTOOL_H
