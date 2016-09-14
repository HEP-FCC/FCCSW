#ifndef SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H
#define SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
namespace CLHEP{
   class Hep3Vector;
}

/** @class ISimG4ParticleSmearTool SimG4Interface/SimG4Interface/ISimG4ParticleSmearTool.h ISimG4ParticleSmearTool.h
 *
 *  Interface to the particle smearing tool.
 *  (smearing its momentum/energy for the purpose of fast simulation)
 *
 *  @author Anna Zaborowska
 */

class ISimG4ParticleSmearTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4ParticleSmearTool,1,0);

  /**  Smear the momentum of the particle
   *   @param aMom Particle momentum to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom, int aPdg = 0) = 0;
  /**  Get the names of the volumes where fast simulation should be performed.
   *   @return vector of volume names
   */
  virtual const std::vector<std::string>& volumeNames() const = 0;
  /**  Get the minimum momentum that triggers fast simulation
   *   @return minimum p
   */
  virtual double minP() const = 0;
  /**  Get the maximum momentum that triggers fast simulation
   *   @return maximum p
   */
  virtual double maxP() const = 0;
  /**  Get the maximum pseudorapidity that triggers fast simulation
   *   @return maximum eta
   */
  virtual double maxEta() const = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H */
