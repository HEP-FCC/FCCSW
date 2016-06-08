#ifndef SIMG4INTERFACE_IG4EVENTPROVIDERTOOL_H
#define SIMG4INTERFACE_IG4EVENTPROVIDERTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4Event;

/** @class IG4EventProviderTool IG4EventProviderTool.h "SimG4Interface/IG4EventProviderTool.h"
 *
 *  Abstract interface to Geant4 Primary Generator classes
 *
 *  @author Andrea Dell'Acqua, J. Lingemann
 *  @date   2014-10-02
 */

class IG4EventProviderTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IG4EventProviderTool, 1, 0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /** get initilization hook for the geometry
   *  @return pointer to the G4Event containing primary particles
   */
  virtual G4Event* g4Event() = 0;
};
#endif
