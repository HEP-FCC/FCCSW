#ifndef SIMG4FAST_G4PARTICLESMEARFORMULA_H
#define SIMG4FAST_G4PARTICLESMEARFORMULA_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class IRndmGen;

//ROOT
class TFormula;

// FCCSW
#include "SimG4Interface/IG4ParticleSmearTool.h"

/** @class G4ParticleSmearFormula SimG4Fast/src/components/G4ParticleSmearFormula.h G4ParticleSmearFormula.h
 *
 *  Formula particle smearing tool.
 *  Momentum/energy dependent smearing.
 *  The resolution dependence can be expressed by an arbitrary formula in the configuration.
 *  Smears momentum/energy of the particle following a Gaussian distribution, using the evaluated formula as the mean.
 *
 *  @author Anna Zaborowska
 */

class G4ParticleSmearFormula: public GaudiTool, virtual public IG4ParticleSmearTool {
public:
  explicit G4ParticleSmearFormula(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~G4ParticleSmearFormula();

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
  /// string defining a TFormula representing resolution energy-dependent for the smearing (set by job options)
  std::string m_resolutionEnergyStr;
  /// string defining a TFormula representing resolution momentum-dependent for the smearing (set by job options)
  std::string m_resolutionMomentumStr;
  /// TFormula representing resolution energy-dependent for the smearing
  std::unique_ptr<TFormula> m_resolutionEnergy;
  /// TFormula representing resolution momentum-dependent for the smearing
  std::unique_ptr<TFormula> m_resolutionMomentum;
  /// Random Number Service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  IRndmGen* m_gauss;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
