#ifndef SIMG4INTERFACE_ISIMG4GFLASHTOOL_H
#define SIMG4INTERFACE_ISIMG4GFLASHTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
#include "GVFlashShowerParameterisation.hh"

/** @class ISimG4GflashTool SimG4Interface/SimG4Interface/ISimG4GflashTool.h ISimG4GflashTool.h
 *
 *  Interface to the Gflash parametrisation tool.
 *  It returns the parametriation that should be attached to the GFlashShowerModel.
 *
 *  @author Anna Zaborowska
 */

class ISimG4GflashTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4GflashTool, 1, 0);

  /**  Get the parametrisation
   *   @return unique pointer to the parametrisation
   */
  virtual std::unique_ptr<GVFlashShowerParameterisation> parametrisation() = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4GFLASHTOOL_H */
