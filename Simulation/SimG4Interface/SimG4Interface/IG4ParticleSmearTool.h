#ifndef SIMG4INTERFACE_IG4PARTICLESMEARTOOL_H
#define SIMG4INTERFACE_IG4PARTICLESMEARTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
namespace CLHEP{
   class Hep3Vector;
}

/** @class IG4ParticleSmearTool SimG4Interface/SimG4Interface/IG4ParticleSmearTool.h IG4ParticleSmearTool.h
 *
 *  Interface to the particle smearing tool.
 *  (smearing its momentum/energy for the purpose of fast simulation)
 *
 *  @author Anna Zaborowska
 */

class IG4ParticleSmearTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(IG4ParticleSmearTool,1,0);

  /**  Smear the momentum of the particle
   *   @param aMom Particle momentum to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom, int aPdg = 0) = 0;

  /**  Smear the energy of the particle
   *   @param aEn Particle energy to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearEnergy(double& aEn, int aPdg = 0) = 0;
};
#endif /* SIMG4INTERFACE_IG4PARTICLESMEARTOOL_H */
