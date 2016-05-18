#ifndef Sim_IG4ParticleGeneratorTool_H
#define Sim_IG4ParticleGeneratorTool_H

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4VUserPrimaryGeneratorAction;

/** @class IG4ParticleGeneratorTool IG4ParticleGeneratorTool.h "SimG4Interface/IG4ParticleGeneratorTool.h"
 *  
 *  Abstract interface to Geant4 Primary Generator classes
 * 
 *  @author Andrea Dell'Acqua
 *  @date   2014-10-02
 */

static const InterfaceID IID_IG4ParticleGeneratorTool( "IG4ParticleGeneratorTool" , 1 , 0 ) ;

class IG4ParticleGeneratorTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() { return IID_IG4ParticleGeneratorTool ; }

  /**  initialize
   *   @return status code
   */
  virtual StatusCode   initialize() = 0 ;

  /** get initilization hook for the geometry
   *  @return pointer to G4VUserPrimaryGeneratorAction
   */
  virtual G4VUserPrimaryGeneratorAction* getParticleGenerator() = 0 ;
};
#endif
