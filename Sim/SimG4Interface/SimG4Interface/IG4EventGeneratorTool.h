#ifndef SIMG4INTERFACE_IG4EVENTGENERATORTOOL_H
#define SIMG4INTERFACE_IG4EVENTGENERATORTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4Event;

/** @class IG4EventGeneratorTool IG4EventGeneratorTool.h "SimG4Interface/IG4EventGeneratorTool.h"
 *
 *  Abstract interface to Geant4 Primary Generator classes
 *
 *  @author Andrea Dell'Acqua, J. Lingemann
 *  @date   2014-10-02
 */


class IG4EventGeneratorTool : virtual public IAlgTool {
 public:
  DeclareInterfaceID(IG4EventGeneratorTool,1,0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode   initialize() = 0 ;

  /** get initilization hook for the geometry
   *  @return pointer to the G4Event containing primary particles
   */
  virtual G4Event* g4Event() = 0 ;
};
#endif
