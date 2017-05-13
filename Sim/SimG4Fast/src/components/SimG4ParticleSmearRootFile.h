#ifndef SIMG4FAST_G4PARTICLESMEARROOTFILE_H
#define SIMG4FAST_G4PARTICLESMEARROOTFILE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;
class IRndmGen;

// ROOT
#include "TGraph.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

/** @class SimG4ParticleSmearRootFile SimG4Fast/src/components/SimG4ParticleSmearRootFile.h SimG4ParticleSmearRootFile.h
 *
 *  Root file particle smearing tool.
 *  The resolution dependence is read from the ROOT file, which path is given in configuration.
 *  Root file contains trees 'info' and 'resolutions'.
 *  'info' has two arrays of type TArrayD containing edges of eta bins ('eta') and momentum values ('p').
 *  'resolutions' tree has TArrayD with resolutions computed for momentum values. An array is defined for every eta bin.
 *  Momentum of the particle is smeared following a Gaussian distribution,
 *  using the evaluated resolution as the mean.
 *  User needs to specify the min/max momentum nad max eta for fast sim in the `SimG4FastSimTrackerRegion` tool.
 *  The defined values cannot be broader than eta and p values for which the resolutions were computed.
 *
 *  @author Anna Zaborowska
 */

class SimG4ParticleSmearRootFile : public GaudiTool, virtual public ISimG4ParticleSmearTool {
public:
  explicit SimG4ParticleSmearRootFile(const std::string& type, const std::string& name, const IInterface* parent);
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

  /**  Check conditions of the smearing model, especially if the given parametrs do not exceed the parameters of the
   * model.
   *   @param[in] aMinMomentum Minimum momentum.
   *   @param[in] aMaxMomentum Maximum momentum.
   *   @param[in] aMaxEta Maximum pseudorapidity.
   *   @return status code
   */
  virtual StatusCode checkConditions(double aMinMomentum, double aMaxMomentum, double aMaxEta) const final;

private:
  /// Random Number Service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  IRndmGen* m_gauss;
  /// Map of p-dependent resolutions and the end of eta bin that it refers to
  /// (lower end is defined by previous entry, and eta=0 for the first one)
  std::map<double, TGraph> m_momentumResolutions;
  /// File name with the resolutions obtained from root file (set by job options)
  Gaudi::Property<std::string> m_resolutionFileName{this, "filename", "",
                                                    "File name with the resolutions obtained from root file"};
  /// minimum momentum defined in the resolution file
  double m_minMomentum;
  /// maximum momentum defined in the resolution file
  double m_maxMomentum;
  /// maximum pseudorapidity defined in the resolution file
  double m_maxEta;
};

#endif /* SIMG4FAST_G4PARTICLESMEARROOTFILE_H */
