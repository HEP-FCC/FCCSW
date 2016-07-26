#ifndef SIMG4FAST_G4PARTICLESMEARSIMPLE_H
#define SIMG4FAST_G4PARTICLESMEARSIMPLE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

/** @class SimG4ParticleSmearSimple SimG4Fast/src/components/SimG4ParticleSmearSimple.h SimG4ParticleSmearSimple.h
 *
 *  Simple particle smearing tool.
 *  Smears the momentum of the particle following the Gaussian distribution.
 *  The standard deviation of the Gaussian is set in the job options file ('sigma').
 *  User may define in job options the momentum range (\b'minP', \b'maxP') and the maximum pseudorapidity (\b'maxEta')
 *  for which the fast simulation is triggered (for other particles full simulation is performed).
 *
 *  @author Anna Zaborowska
 */

class SimG4ParticleSmearSimple: public GaudiTool, virtual public ISimG4ParticleSmearTool {
public:
  explicit SimG4ParticleSmearSimple(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~SimG4ParticleSmearSimple();

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
  /// Constant resolution for the smearing (set by job options)
  double m_sigma;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
  /// minimum P that can be smeared
  double m_minP;
  /// maximum P that can be smeared
  double m_maxP;
  /// maximum eta that can be smeared
  double m_maxEta;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
