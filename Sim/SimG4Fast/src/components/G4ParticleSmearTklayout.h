#ifndef SIMG4FAST_G4PARTICLESMEARTKLAYOUT_H
#define SIMG4FAST_G4PARTICLESMEARTKLAYOUT_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

//ROOT
#include "TGraph.h"

// FCCSW
#include "SimG4Interface/IG4ParticleSmearTool.h"

/** @class G4ParticleSmearTklayout SimG4Fast/src/components/G4ParticleSmearTklayout.h G4ParticleSmearTklayout.h
 *
 *  Tklayout particle smearing tool.
 *  Smears the momentum/energy of the particle following the Gaussian distribution.
 *  TODO about where resolutions come from
 *
 *  @author Anna Zaborowska
 */

class G4ParticleSmearTklayout: public GaudiTool, virtual public IG4ParticleSmearTool {
public:
  explicit G4ParticleSmearTklayout(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~G4ParticleSmearTklayout();

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
  /**  Read the file with the resolutions. File name is set by job options.
   *   @return status code
   */
  StatusCode readResolutions();
  /**  Read the file with the resolutions. File name is set by job options.
   *   @param[in] aEta Particle's pseudorapidity
   *   @param[in] aMom Particle's momentum
   *   @return Resolution
   */
  double resolution(double aEta, double aMom);
private:
  /// Map of p-dependent resolutions and the end of eta bin that it refers to
  /// (lower end is defined by previous entry, and eta=0 for the first one)
  std::map<double, std::unique_ptr<TGraph>> m_momentumResolutions;
  /// Maximum eta for which resolutions are defined (filled at the end of file reading)
  double m_maxEta;
  /// File name with the resolutions obtained from tkLayout (set by job options)
  std::string m_resolutionFileName;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* SIMG4FAST_G4PARTICLESMEARSIMPLE_H */
