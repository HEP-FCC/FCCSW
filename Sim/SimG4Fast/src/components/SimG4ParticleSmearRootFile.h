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
 *  Momentum/energy dependent smearing.
 *  The resolution dependence is read from the ROOT file, which path is given in configuration.
 *  Root file should contain (for each pseudorapidity bin) TGraph with p-dependent resolution.
 *  The name of the TGraph should follow the convention: "etafromX_etatoY", where X and Y are
 *  lower and upper boundaries of eta bin.
 *  Momentum/energy of the particle is smeared following a Gaussian distribution,
 *  using the evaluated resolution as the mean.
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
  /// File name with the resolutions obtained from root file (set by job options)
  std::string m_resolutionFileName;
  /// Random Number Service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  IRndmGen* m_gauss;
};

#endif /* SIMG4FAST_G4PARTICLESMEARROOTFILE_H */
