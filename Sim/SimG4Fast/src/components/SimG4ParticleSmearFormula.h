#ifndef SIMG4FAST_G4PARTICLESMEARFORMULA_H
#define SIMG4FAST_G4PARTICLESMEARFORMULA_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class IRndmGen;

//ROOT
#include "TFormula.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

/** @class SimG4ParticleSmearFormula SimG4Fast/src/components/SimG4ParticleSmearFormula.h SimG4ParticleSmearFormula.h
 *
 *  Formula particle smearing tool.
 *  Momentum dependent smearing performed in the volumes as specified in the job options (\b'detectorNames').
 *  The resolution dependence can be expressed by an arbitrary formula in the configuration.
 *  Smears momentum of the particle following a Gaussian distribution, using the evaluated formula as the mean.
 *  User may define in job options the momentum range (\b'minP', \b'maxP') and the maximum pseudorapidity (\b'maxEta')
 *  for which the fast simulation is triggered (for other particles full simulation is performed).
 *
 *  @author Anna Zaborowska
 */

class SimG4ParticleSmearFormula: public GaudiTool, virtual public ISimG4ParticleSmearTool {
public:
  explicit SimG4ParticleSmearFormula(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~SimG4ParticleSmearFormula();

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
  /**  Get the names of the volumes where fast simulation should be performed.
   *   @return vector of volume names
   */
  inline virtual const std::vector<std::string>* volumeNames() const final {return &m_volumeNames;};
  /**  Get the minimum momentum that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double minP() const final {return m_minP;};
  /**  Get the maximum momentum that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double maxP() const final {return m_maxP;};
  /**  Get the maximum pseudorapidity that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double maxEta() const final {return m_maxEta;};

private:
  /// TFormula representing resolution momentum-dependent for the smearing
  TFormula m_resolutionMomentum;
  /// Random Number Service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  IRndmGen* m_gauss;
  /// Names of the parametrised volumes (set by job options)
  std::vector<std::string> m_volumeNames;
  /// string defining a TFormula representing resolution momentum-dependent for the smearing (set by job options)
  std::string m_resolutionMomentumStr;
  /// minimum P that can be smeared (set by job options)
  double m_minP;
  /// maximum P that can be smeared (set by job options)
  double m_maxP;
  /// maximum eta that can be smeared (set by job options)
  double m_maxEta;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
