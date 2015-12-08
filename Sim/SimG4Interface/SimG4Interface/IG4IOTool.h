#ifndef SIM_IG4IOTOOL_H
#define SIM_IG4IOTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class sim::IG4IOTool SimG4Interface/IG4IOTool.h IG4IOTool.h
 *
 *  An interface to define inputs and outputs of Geant4 simulation.
 *
 *  @author Anna Zaborowska
 */

static const InterfaceID IID_IG4IOTool("IG4IOTool",1,0);

class IG4IOTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IG4IOTool; }

  /**  save the output
   *   @return status code
   */
  virtual StatusCode saveOutput() = 0;
};
#endif /* SIM_IG4IOTOOL_H */
