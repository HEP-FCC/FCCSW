#ifndef SIMG4FAST_G4PARTICLESMEARSIMPLE_H
#define SIMG4FAST_G4PARTICLESMEARSIMPLE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "SimG4Interface/IG4ParticleSmearTool.h"

/** @class G4ParticleSmearSimple SimG4Fast/src/components/G4ParticleSmearSimple.h G4ParticleSmearSimple.h
 *
 *  Simple particle smearing tool.
 *  Smears the momentum/energy of the particle following the Gaussian distribution.
 *  The standard deviation of the Gaussian is set in the job options file ('sigma').
 *
 *  @author Anna Zaborowska
 */

class G4ParticleSmearSimple: public GaudiTool, virtual public IG4ParticleSmearTool {
public:
  explicit G4ParticleSmearSimple(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~G4ParticleSmearSimple();

  /**  Initialize the tool and a random number generator.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

  /**  Smear the momentum of the particle
   *   @param aMom Particle momentum to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom, int aPdg = 0) final;

  /**  Smear the energy of the particle
   *   @param aEn Particle energy to be smeared.
   *   @param[in] aPdg Particle PDG code.
   *   @return status code
   */
  virtual StatusCode smearEnergy(double& aEn, int aPdg = 0) final;
private:
  /// Constant resolution for the smearing (set by job options)
  double m_sigma;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
