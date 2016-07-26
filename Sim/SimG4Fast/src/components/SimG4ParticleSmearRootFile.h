#ifndef SIMG4FAST_G4PARTICLESMEARROOTFILE_H
#define SIMG4FAST_G4PARTICLESMEARROOTFILE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class IRndmGen;

//ROOT
#include "TGraph.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

/** @class SimG4ParticleSmearRootFile SimG4Fast/src/components/SimG4ParticleSmearRootFile.h SimG4ParticleSmearRootFile.h
 *
 *  Root file particle smearing tool.
 *  Momentum dependent smearing.
 *  The resolution dependence is read from the ROOT file, which path is given in configuration.
 *  Root file contains trees 'info' and 'resolutions'.
 *  'info' has two arrays of type TArrayD containing edges of eta bins ('eta') and momentum values ('p').
 *  'resolutions' tree has TArrayD with resolutions computed for momentum values. An array is defined for every eta bin.
 *  Momentum of the particle is smeared following a Gaussian distribution,
 *  using the evaluated resolution as the mean.
 *  User may define in job options the momentum range (\b'minP', \b'maxP') and the maximum pseudorapidity (\b'maxEta')
 *  for which the fast simulation is triggered (for other particles full simulation is performed).
 *  The defined values cannot be broader than eta and p values for which the resolutions were computed.
 *
 *  @author Anna Zaborowska
 */

class SimG4ParticleSmearRootFile: public GaudiTool, virtual public ISimG4ParticleSmearTool {
public:
  explicit SimG4ParticleSmearRootFile(const std::string& type , const std::string& name,
    const IInterface* parent);
  virtual ~SimG4ParticleSmearRootFile();

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
  std::map<double, TGraph> m_momentumResolutions;
  /// File name with the resolutions obtained from root file (set by job options)
  std::string m_resolutionFileName;
  /// Random Number Service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  IRndmGen* m_gauss;
  /// minimum P that can be smeared
  double m_minP;
  /// maximum P that can be smeared
  double m_maxP;
  /// maximum eta that can be smeared
  double m_maxEta;
};

#endif /* SIMG4FAST_G4PARTICLESMEARROOTFILE_H */
