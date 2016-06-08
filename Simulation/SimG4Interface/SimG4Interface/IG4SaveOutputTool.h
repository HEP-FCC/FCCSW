#ifndef SIMG4INTERFACE_IG4SAVEOUTPUTTOOL_H
#define SIMG4INTERFACE_IG4SAVEOUTPUTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant
class G4Event;


/** @class IG4SaveOutputTool SimG4Interface/SimG4Interface/IG4SaveOutputTool.h IG4SaveOutputTool.h
 *
 *  Interface to the output saving tool.
 *
 *  @author Anna Zaborowska
 */

class IG4SaveOutputTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(IG4SaveOutputTool,1,0);

  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) = 0;
};
#endif /* SIMG4INTERFACE_IG4SAVEOUTPUTTOOL_H */
