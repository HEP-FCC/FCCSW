#ifndef SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H
#define SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
class G4Event;

/** @class ISimG4SaveOutputTool SimG4Interface/SimG4Interface/ISimG4SaveOutputTool.h ISimG4SaveOutputTool.h
 *
 *  Interface to the output saving tool.
 *
 *  @author Anna Zaborowska
 */

class ISimG4SaveOutputTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4SaveOutputTool, 1, 0);

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4SAVEOUTPUTTOOL_H */
