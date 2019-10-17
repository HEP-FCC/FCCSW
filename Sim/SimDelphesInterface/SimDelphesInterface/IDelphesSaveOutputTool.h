#ifndef SIMDELPHESINTERFACE_IDELPHESAVEOUTPUTTOOL_H
#define SIMDELPHESINTERFACE_IDELPHESAVEOUTPUTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// ROOT
class Delphes;
namespace fcc {
class MCParticleCollection;
}

/** @class IDelphesSaveOutputTool SimG4Interface/SimG4Interface/IDelphesSaveOutputTool.h IDelphesSaveOutputTool.h
 *
 *  Interface for a tool to save Delphes collection to FCC EDM.
 *
 *  @author J. Lingemann
 */

class IDelphesSaveOutputTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IDelphesSaveOutputTool, 1, 0);

  /**  Save the certain objects from Delphes to EDM.
   *   @param[in] delphes: Delphes module from which we can retrieve the output.
   *   @param[in] mcParticles: EDM collection holding MC particles to establish relations.
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) = 0;
};
#endif /* SIMDELPHESINTERFACE_IDELPHESAVEOUTPUTTOOL_H */
