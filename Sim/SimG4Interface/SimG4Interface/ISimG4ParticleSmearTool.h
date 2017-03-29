#ifndef SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H
#define SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
namespace CLHEP {
class Hep3Vector;
}

/** @class ISimG4ParticleSmearTool SimG4Interface/SimG4Interface/ISimG4ParticleSmearTool.h ISimG4ParticleSmearTool.h
 *
 *  Interface to the particle smearing tool.
 *  (smearing its momentum/energy for the purpose of fast simulation)
 *
 *  @author Anna Zaborowska
 */

class ISimG4ParticleSmearTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4ParticleSmearTool, 1, 0);

  /**  Smear the momentum of the particle
   *   @param aMom Particle momentum to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom, int aPdg = 0) = 0;

  /**  Check conditions of the smearing model, especially if the given parametrs do not exceed the parameters of the
   * model.
   *   @param[in] aMinMomentum Minimum momentum.
   *   @param[in] aMaxMomentum Maximum momentum.
   *   @param[in] aMaxEta Maximum pseudorapidity.
   *   @return status code
   */
  virtual StatusCode checkConditions(double aMinMomentum, double aMaxMomentum, double aMaxEta) const = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4PARTICLESMEARTOOL_H */
